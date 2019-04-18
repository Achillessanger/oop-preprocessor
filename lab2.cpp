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

string define_replace_process(string raw_code);

string ifdef_process(vector<string> raws, int* ip);

string ifndef_process(vector<string> raws, int* ip);

string if_process(vector<string> raws, int* ip);

map<string,string> define_build_map(string key, string value);
string processor_jumptable(vector<string> raws, int* ip);
bool need_process(string raw_code);
void sort(string key);
void built_define_map(string raw_code);

const string DEFINDED = "havevalue";
stack<bool> if_stack;
vector<string> order_vec;
map<string,string> define_map;

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
    vector<string> raws = split(raw_code,"\n");


    for(int i = 0; i < raws.size(); i++){
        processed_code += processor_jumptable(raws,&i);
    }



    //debug!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    cout<<" "<<endl;
    for(map<string,string>::iterator it = define_map.begin(); it != define_map.end(); it++){
        cout<<it->first+"  "+it->second<<endl;
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

string define_replace_process(string raw_code){
//    string buf;
//    stringstream ss(raw_code);
//    vector<string> tokens;
//    bool left_quot = false;
//    string tmp = "";
//    while (ss >> buf){
//        //寮曞彿鍐呬笉鏇挎崲
//        if(buf == "\"" and left_quot == false){
//            tmp = "";
//            left_quot = true;
//            tmp += buf;
//            continue;
//        } else if(buf != "\"" and left_quot){
//            tmp += buf;
//            continue;
//        } else if(buf == "\"" and left_quot){
//            tmp += buf;
//            tokens.push_back(tmp);
//            left_quot = false;
//            continue;
//        }
//        tokens.push_back(buf);
//    }
//
//    for(int k = 0; k < tokens.size(); k++){
//        if(regex_match(tokens[k],regex("\".*\""))) //寮曞彿閲屼笉棰勫�勭悊
//            continue;
//
//        string no_sign_code = regex_replace(tokens[k],regex("[^A-Za-z0-9]"),"");
//        if(no_sign_code == "")
//            continue;
//        while ((*define_map).find(no_sign_code) != (*define_map).end()){
//            tokens[k] = regex_replace(tokens[k],regex(no_sign_code),((*define_map).find(no_sign_code))->second);
//            no_sign_code = regex_replace(tokens[k],regex("[^A-Za-z0-9]"),"");
//        }
//
//    }
//    string ret;
//    for(int k = 0; k < tokens.size(); k++){
//        ret += " " + tokens[k];
//    }
//    return ret;


    for(int i = 0; i < order_vec.size(); i++){
        if(regex_match(raw_code,regex(".*"+order_vec[i]+".*"))){
            raw_code = regex_replace(raw_code,regex(order_vec[i]),define_map.find(order_vec[i])->second);
        }
    }
    return raw_code;

}

string ifdef_process(vector<string> raws, int* ip){
    int stack_size = if_stack.size();
    string ret;
    string content = regex_replace(raws[*ip],regex("# *ifdef *"),"");
    if_stack.push((define_map.find(content) != define_map.end()));
    (*ip)++;
    bool ignore = !if_stack.top();
    while (stack_size != if_stack.size()){
        if(need_process(raws[*ip])){
            ret += processor_jumptable(raws,ip) + "\n";
            (*ip)++;
            continue;
        }

        if(regex_match(raws[*ip],regex("# *else *"))){
            ignore = !ignore;
            (*ip)++;
            continue;
        }
        if(regex_match(raws[*ip],regex("# *endif *"))) {
            if_stack.pop();
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

string ifndef_process(vector<string> raws, int* ip){
    int stack_size = if_stack.size();
    string ret;
    string content = regex_replace(raws[*ip],regex("# *ifndef *"),"");
    if_stack.push((define_map.find(content) == define_map.end()));
    (*ip)++;
    bool ignore = !if_stack.top();
    while (stack_size != if_stack.size()){
        if(need_process(raws[*ip])){
            ret += processor_jumptable(raws,ip) + "\n";
            (*ip)++;
            continue;
        }

        if(regex_match(raws[*ip],regex("# *else *"))){
            ignore = !ignore;
            (*ip)++;
            continue;
        }
        if(regex_match(raws[*ip],regex("# *endif *"))) {
            if_stack.pop();
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

string if_process(vector<string> raws, int* ip){
    int stack_size = if_stack.size();
    string ret;
    string content = regex_replace(raws[*ip],regex("# *if *"),"");
    if(content == "TRUE" || content == "1"){
        if_stack.push(true);
    } else{
        if_stack.push(false);
    }
    (*ip)++;
    bool ignore = !if_stack.top();
    while (stack_size != if_stack.size()){
        if(need_process(raws[*ip])){
            ret += processor_jumptable(raws,ip) + "\n";
            (*ip)++;
            continue;
        }

        if(regex_match(raws[*ip],regex("# *else *"))){
            ignore = !ignore;
            (*ip)++;
            continue;
        }
        if(regex_match(raws[*ip],regex("# *endif *"))) {
            if_stack.pop();
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




void sort(string key){
    vector<string>::iterator it = order_vec.begin();
    for(; it != order_vec.end(); it++){
        if(key.length() < it->length())
            continue;
        else{
            order_vec.insert(it,1,key);
            return;
        }
    }
    order_vec.push_back(key);
    return;
}

string processor_jumptable(vector<string> raws, int* ip){
    string processed_code = "";
    if(regex_match(raws[*ip],regex("# *include *.*")))
        processed_code += include_process(raws[*ip]) + "\n";
    else if(regex_match(raws[*ip],regex("# *define *.*"))){
        built_define_map(raws[*ip]);
    } else if(regex_match(raws[*ip],regex("# *undef *.*"))){
        define_map.erase(regex_replace(raws[*ip],regex("# *undef *"),""));
    } else if(regex_match(raws[*ip],regex("# *ifdef *.*"))){
        processed_code += ifdef_process(raws,ip) + "\n";
    } else if(regex_match(raws[*ip],regex("# *ifndef *.*"))){
        processed_code += ifndef_process(raws,ip) + "\n";
    } else if(regex_match(raws[*ip],regex("# *if *.*"))){
        processed_code += if_process(raws,ip) + "\n";
    } else if(regex_match(raws[*ip],regex(" *//.*"))){
        processed_code += raws[*ip] + "\n";
    }else{
        if(raws[*ip] == ""){
            processed_code += "\n";
        }else
            processed_code += define_replace_process(raws[*ip]) + "\n";
    }
    return processed_code;
}

bool need_process(string raw_code){
    if(regex_match(raw_code,regex("# *include *.*"))||
       regex_match(raw_code,regex("# *define *.*"))||
       regex_match(raw_code,regex("# *undef *.*"))||
       regex_match(raw_code,regex("# *ifdef *.*"))||
       regex_match(raw_code,regex("# *ifndef *.*"))||
       regex_match(raw_code,regex("# *if *.*"))||
       regex_match(raw_code,regex(" *//.*")))
        return true;
    else
        return false;
}

void built_define_map(string raw_code){
    string tmp = regex_replace(raw_code,regex("# *define *"),"");
    if(tmp.find(" ") == string::npos){ //PART1 PART2 PART3
        define_map[tmp] = DEFINDED;
        sort(tmp);
    } else{
        string key = tmp.substr(0,tmp.find(" "));
        string value = tmp.substr(tmp.find(" ")+1);

        for(int k = 0; k < order_vec.size(); k++){
            if(regex_match(value,regex(".*"+order_vec[k]+".*"))){
                value = regex_replace(value,regex(order_vec[k]),define_map.find(order_vec[k])->second);
            }
        }
        define_map[key] = value;
        sort(key);
    }
}