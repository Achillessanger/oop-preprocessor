#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <map>
#include <sstream>
#include <stack>
using namespace std;

string get_unprocessed_code(int number);

void put_processed_code(int number, string code);

void run_test(int test_case_number);

vector<string> split(string input, string delimiter);

string include_process(string raw_code);

string define_replace_process(string raw_code, map<string,string> define_map);

string ifdef_process(vector<string> raws, int* ip, map<string,string> define_map);

string ifndef_process(vector<string> raws, int* ip, map<string,string> define_map);

string if_process(vector<string> raws, int* ip, map<string,string> define_map);

map<string,string> define_build_map(string key, string value, map<string,string> map);

vector<string> sort(map<string,string> mapp);


int main() {
    for (int test_case_number = 1; test_case_number <= 2; test_case_number++) {
        run_test(test_case_number);
    }
    return 0;
}

void run_test(int test_case_number) {
    string raw_code = get_unprocessed_code(test_case_number);

    /*
     * TODO: Your Code Here!
     * TODO: Take raw_code as your input, and output your processed code.
     * TODO: You'd better create new classes to handle your logic and use here only as an entrance.
     * */
    string processed_code;
    map<string,string> define_map;
//    define_map["1"] = "??";
//    define_map.insert(make_pair("3","MM") );
//
//    cout << define_map.size();
//
//
//    if(define_map.find("1") != define_map.end())
//        cout<<"Find, the value is"<<define_map.find("1")->second<<endl;
//    else
//        cout<<"Do not Find"<<endl;
//    map<string,string>::iterator it = define_map.begin();
//    while (it != define_map.end()){
//        cout<<it->first<<endl;
//        it++;
//    }


    const string DEFINDED = "havevalue";
    vector<string> raws = split(raw_code,"\n");
    for(int i = 0; i < raws.size(); i++){
        //#开头的
        if(regex_match(raws[i],regex("# *include *.*")))
            processed_code += include_process(raws[i]) + "\n";
        else if(regex_match(raws[i],regex("# *define *.*"))){
            string tmp = regex_replace(raws[i],regex("# *define *"),"");
            if(tmp.find(" ") == string::npos){ //PART1 PART2 PART3
                define_map[tmp] = DEFINDED;
            } else{
                string key = tmp.substr(0,tmp.find(" "));
                string value = tmp.substr(tmp.find(" ")+1);
                vector<string> order_vec = sort(define_map);
                for(int k = 0; k < order_vec.size(); k++){
                    if(regex_match(value,regex(".*"+order_vec[k]+".*"))){
                        value = regex_replace(value,regex(order_vec[k]),define_map.find(order_vec[k])->second);
                    }
                }
                define_map[key] = value;
            }

        } else if(regex_match(raws[i],regex("# *undef *.*"))){
            define_map.erase(regex_replace(raws[i],regex("# *undef *"),""));
        } else if(regex_match(raws[i],regex("# *ifdef *.*"))){
            processed_code += ifdef_process(raws,&i,define_map) + "\n";
        } else if(regex_match(raws[i],regex("# *ifndef *.*"))){
            processed_code += ifndef_process(raws,&i,define_map) + "\n";
        } else if(regex_match(raws[i],regex("# *if *.*"))){
            processed_code += if_process(raws,&i,define_map) + "\n";
        } else if(regex_match(raws[i],regex(" *//.*"))){
            processed_code += raws[i] + "\n";
        }else{
            if(raws[i] == ""){
                processed_code += "\n";
                continue;
            }
            processed_code += define_replace_process(raws[i],define_map) + "\n";
        }
    }

    if(define_map.find("TRUE") != define_map.end()){
        cout<<define_map.find("TRUE")->second<<endl;
    }



    define_map.clear();
    put_processed_code(test_case_number, processed_code);
}

string get_unprocessed_code(int number) {
    string filename = "../test/test" + to_string(number) + ".cpp";
    string file;
    ifstream is(filename);
    if (!is.is_open()) {
        cout << "Broken input file.";
    } else {
        string line;
        while (getline(is, line)) {
            file.append(line).push_back('\n');
        }
        is.close();
    }
    return file;
}

void put_processed_code(int number, string code) {
    string filename = "../test/test" + to_string(number) + ".out.cpp";
    ofstream os(filename);
    if (!os.is_open()) {
        cout << "Broken output file.";
    } else {
        os << code;
        os.close();
    }
}

vector<string> split(string input, string delimiter){
    vector<string> result;
    string str = input;
    while(!str.empty()){
        int ind = str.find_first_of(delimiter);
        if(ind == -1){
            result.push_back(str);
            str.clear();
        }
        else{
            result.push_back(str.substr(0, ind));
            str = str.substr(ind+1);
        }
    }
    return result;
}

string include_process(string raw_code){
    string ret = raw_code;
    raw_code = regex_replace(raw_code,regex("# *include *"),"");
    raw_code = regex_replace(raw_code,regex("<|>|\""),"");
    if(raw_code == "iostream")
        return ret;
    else{
        string filename = "../test/" + raw_code;
        string file;
        ifstream is(filename);
        if (!is.is_open()) {
            cout << "Broken include file.";
        } else {
            string line;
            while (getline(is, line)) {
                file.append(line).push_back('\n');
            }
            is.close();
        }
        return file;
    }
}

string define_replace_process(string raw_code, map<string,string> define_map){
    string buf;
    stringstream ss(raw_code);
    vector<string> tokens;
    bool left_quot = false;
    string tmp = "";
    while (ss >> buf){
        //引号内不替换
        if(buf == "\"" and left_quot == false){
            tmp = "";
            left_quot = true;
            tmp += buf;
            continue;
        } else if(buf != "\"" and left_quot){
            tmp += buf;
            continue;
        } else if(buf == "\"" and left_quot){
            tmp += buf;
            tokens.push_back(tmp);
            left_quot = false;
            continue;
        }
        tokens.push_back(buf);
    }

    for(int k = 0; k < tokens.size(); k++){
        if(regex_match(tokens[k],regex("\".*\""))) //引号里不预处理
            continue;

        string no_sign_code = regex_replace(tokens[k],regex("[^A-Za-z0-9]"),"");
        if(no_sign_code == "")
            continue;
        while (define_map.find(no_sign_code) != define_map.end()){
            tokens[k] = regex_replace(tokens[k],regex(no_sign_code),(define_map.find(no_sign_code))->second);
            no_sign_code = regex_replace(tokens[k],regex("[^A-Za-z0-9]"),"");
        }

    }
    string ret;
    for(int k = 0; k < tokens.size(); k++){
        ret += " " + tokens[k];
    }
    return ret;


//    map<string,string>::iterator it = define_map.begin();
//    string debugg = raw_code;
//    while (it != define_map.end()){
//        cout<<it->first+"  "+it->second[0]<<endl;
//        if(regex_match(raw_code,regex(".*"+it->first+".*")) &&
//           !regex_match(raw_code.substr(0,raw_code.find(it->first)),regex(".*[a-zA-Z0-9_]")) &&
//           !regex_match(raw_code.substr(raw_code.find(it->first) + it->first.length()),regex("[a-zA-Z0-9_].*"))){
//            raw_code = regex_replace(raw_code,regex(it->first),it->second);
//        }
//        it++;
//    }
//    return raw_code;

}

string ifdef_process(vector<string> raws, int* ip, map<string,string> define_map){
    stack<bool> if_stack;
    string ret;
    string content = regex_replace(raws[*ip],regex("# *ifdef *"),"");
    if_stack.push((define_map.find(content) != define_map.end()));
    (*ip)++;
    bool ignore = !if_stack.top();
    while (!if_stack.empty()){
        if(regex_match(raws[*ip],regex("# *ifdef *"))){
            content = regex_replace(raws[*ip],regex("# *ifdef *"),"");
            if_stack.push((define_map.find(content) != define_map.end()));
            (*ip)++;
            continue;
        } else if(regex_match(raws[*ip],regex("# *endif *"))){
            if_stack.pop();
            (*ip)++;
            continue;
        }

        if(regex_match(raws[*ip],regex("# *else *"))){
            ignore = !ignore;
            (*ip)++;
            continue;
        }

        if(!ignore){
            ret += raws[*ip] + "\n";
        }
        (*ip)++;
    }
    (*ip)--;
    return ret;
}

string ifndef_process(vector<string> raws, int* ip, map<string,string> define_map){
    stack<bool> if_stack;
    string ret;
    string content = regex_replace(raws[*ip],regex("# *ifdef *"),"");
    if_stack.push((define_map.find(content) == define_map.end()));
    (*ip)++;
    bool ignore = !if_stack.top();
    while (!if_stack.empty()){
        if(regex_match(raws[*ip],regex("# *ifdef *"))){
            content = regex_replace(raws[*ip],regex("# *ifdef *"),"");
            if_stack.push((define_map.find(content) == define_map.end()));
            (*ip)++;
            continue;
        } else if(regex_match(raws[*ip],regex("# *endif *"))){
            if_stack.pop();
            (*ip)++;
            continue;
        }

        if(regex_match(raws[*ip],regex("# *else *"))){
            ignore = !ignore;
            (*ip)++;
            continue;
        }

        if(!ignore){
            ret += raws[*ip] + "\n";
        }
        (*ip)++;
    }
    (*ip)--;
    return ret;
}

string if_process(vector<string> raws, int* ip, map<string,string> define_map){
    stack<bool> if_stack;
    string ret;
    string content = regex_replace(raws[*ip],regex("# *if *"),"");
    if(content == "TRUE" || content == "1"){
        if_stack.push(true);
    } else{
        if_stack.push(false);
    }
    (*ip)++;
    bool ignore = !if_stack.top();
    while (!if_stack.empty()){
        if(regex_match(raws[*ip],regex("# *ifdef *"))){
            content = regex_replace(raws[*ip],regex("# *ifdef *"),"");
            if(content == "TRUE" || content == "1"){
                if_stack.push(true);
            } else{
                if_stack.push(false);
            }
            (*ip)++;
            continue;
        } else if(regex_match(raws[*ip],regex("# *endif *"))){
            if_stack.pop();
            (*ip)++;
            continue;
        }

        if(regex_match(raws[*ip],regex("# *else *"))){
            ignore = !ignore;
            (*ip)++;
            continue;
        }

        if(!ignore){
            ret += raws[*ip] + "\n";
        }
        (*ip)++;
    }
    (*ip)--;
    return ret;
}

//map<string,string> define_build_map(string key, string value, map<string,string> mapp){
//    cout<<"start"<<endl;
//    map<string,string>::iterator it = mapp.begin();
//    string longest = it->first;
//
//
//}

vector<string> sort(map<string,string> mappp){
//    map<string,string> ret;
    map<string,string> mapp;
    map<string,string>::iterator it;
    for(it = mappp.begin(); it != mappp.end(); it++){
        mapp[it->first] = it->second;
    }

    vector<string> retVec;
    string longest;
    while (mapp.size() != 0){
        longest = mapp.begin()->first;
        for(it = mapp.begin(); it != mapp.end(); it++){
            if(longest.length() < it->first.length()){
                longest = it->first;
            }
        }
//        ret[longest] = mapp.find(longest)->second;
        retVec.push_back(longest);
        mapp.erase(longest);
    }
    return retVec;
}