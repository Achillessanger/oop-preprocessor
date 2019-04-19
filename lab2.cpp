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
vector<string> split_by_regex(string input, regex pattern);
string connect_vector(vector<string> vec);
string piece_replace(string raw_code);
bool need_process(string raw_code);
void sort(string key);
void built_define_map(string raw_code);
vector<string> args_sort(vector<string> args);


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

vector<string> split_by_regex(string input, regex pattern){
    vector<string> result;
    smatch sp;

    string::const_iterator iter = input.begin();
    string::const_iterator iterEnd = input.end();

    while (regex_search(iter,iterEnd,sp,pattern)){
        string a(sp[0].first,sp[0].second);
        result.push_back(a);
        iter = sp[0].second;
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

string piece_replace(string raw_code) {
    for (int i = 0; i < order_vec.size(); i++) {
        //如果define的是函数
        if (regex_match(order_vec[i], regex(".*[(].*[)].*"))) {
            string fun_name = regex_replace(order_vec[i], regex("[(].*"), "");

            if (regex_match(raw_code, regex(".*" + fun_name + ".*"))) {
                string args_list_in_code0 = regex_replace(raw_code, regex(".*[(]|[)].*"), "");
                string args_list_in_code = regex_replace(args_list_in_code0, regex(","), " ");

                vector<string> args_in_code = split_by_regex(args_list_in_code, regex(" *[a-zA-Z0-9_]+ *"));

                string fun_body = define_map.find(order_vec[i])->second;
                for (int k = 0; k < args_in_code.size(); k++) {
                    fun_body = regex_replace(fun_body, regex("arg"+to_string(k)), args_in_code[k]);
                }
                //处理参数里的#,##
                if (regex_match(fun_body, regex(".*##.*"))) {
                    bool left_quot = false;
                    string fun_body2 = fun_body;
                    fun_body = "";

                    while (fun_body2.length() != 0) {
                        string tmp = fun_body2.substr(0, 1);
                        if (!left_quot && tmp == "\"") {
                            left_quot = true;
                        } else if (left_quot && tmp == "\"") {
                            left_quot = false;
                        } else if(!left_quot && tmp == " "){
                            fun_body2 = fun_body2.substr(1);
                            continue;
                        }

                        if (!left_quot && fun_body2.substr(0, 2) == "##") {
                            fun_body2 = fun_body2.substr(2);
                            continue;
                        } else if (left_quot && fun_body2.substr(0, 2) == "##") {
                            fun_body += "##";
                            fun_body2 = fun_body2.substr(2);
                            continue;
                        }
                        fun_body += tmp;
                        fun_body2 = fun_body2.substr(1);
                    }

                } else if (regex_match(fun_body, regex(".*#.*"))) {
                    bool left_quot = false;
                    string fun_body2 = fun_body;
                    fun_body = "";

                    while (fun_body2.length() != 0) {
                        string tmp = fun_body2.substr(0, 1);
                        if (!left_quot && tmp == "\"") {
                            left_quot = true;
                        } else if (left_quot && tmp == "\"") {
                            left_quot = false;
                        }

                        if (!left_quot && tmp == "#") {

                            tmp = "\"" + fun_body2.substr(1, fun_body2.find(" ") - 1) + "\"";
                            fun_body += tmp;
                            if (fun_body2.find(" ") == -1) {
                                fun_body2 = "";
                            } else {
                                fun_body2 = fun_body2.substr(fun_body2.find(" "));
                            }
                            continue;
                        }
                        fun_body += tmp;
                        fun_body2 = fun_body2.substr(1);
                    }

                }

                raw_code = regex_replace(raw_code, regex(fun_name + " *[(]" + args_list_in_code0 + "[)]"), fun_body);
            }
        } else {
            if (regex_match(raw_code, regex(".*" + order_vec[i] + ".*"))) {
                raw_code = regex_replace(raw_code, regex(order_vec[i]), define_map.find(order_vec[i])->second);
            }
        }
    }

    return raw_code;
}

string define_replace_process(string raw_code){

    string raw_code_ret = "";
    bool left_quo = false;
    if(regex_match(raw_code,regex(".*\".*\".*"))){  //如果要替换的地方有引号，引号内不替换
        while (raw_code.length() != 0){
            if(raw_code.find("\"") != -1 && !left_quo){
                left_quo = true;
                raw_code_ret += piece_replace(raw_code.substr(0,raw_code.find("\"")));
                raw_code = raw_code.substr(raw_code.find("\"")+1);
            } else if(raw_code.find("\"") != -1 && left_quo){
                left_quo = false;
                raw_code_ret += "\"" + raw_code.substr(0,raw_code.find("\"")) + "\"";
                if(raw_code.find("\"") != raw_code.length() - 1){
                    raw_code = raw_code.substr(raw_code.find("\"")+1);
                } else{
                    raw_code = "";
                }
            } else if(!left_quo){
                raw_code_ret += piece_replace(raw_code);
                raw_code = "";
            }
        }
    } else{
        raw_code_ret += piece_replace(raw_code);
    }

    return raw_code_ret;
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
            ret += define_replace_process(raws[*ip]) + "\n";
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
            ret += define_replace_process(raws[*ip]) + "\n";
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
            ret += define_replace_process(raws[*ip]) + "\n";
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

vector<string> args_sort(vector<string> args){
    vector<string> ret;
    vector<string>::iterator it;
    ret.push_back(args[0]);
    for(int k = 1; k < args.size(); k++){
        for(it = ret.begin(); it != ret.end(); it++){
            if(args[k].length() < it->length())
                continue;
            else{
                ret.insert(it,1,args[k]);
                break;
            }
        }
    }
    return ret;
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


        if(!regex_match(key,regex(".*[(].*[)].*"))){
            vector<string> replace_1by1 = split_by_regex(value,regex(" *[a-zA-Z0-9_]+[(]{0,1} *"));
            for(int k = 0; k < replace_1by1.size(); k++){
                if(define_map.find(replace_1by1[k]) != define_map.end()){
                    replace_1by1[k] =  define_map.find(replace_1by1[k])->second;
                }
            }
        } else{
            string args_str = regex_replace(key,regex(".*[(]|[)].*"),"");
            vector<string> args = split_by_regex(args_str, regex(" *[a-zA-Z0-9_]+ *"));
            string value0 = "";
            bool left_quo = false;
            vector<string> arg_sorted = args_sort(args);
            if(regex_match(value,regex(".*\".*\".*"))){
                string need_replace;
                while (value.length() != 0){
                    if(value.find("\"") != -1 && !left_quo){
                        left_quo = true;
                        need_replace = value.substr(0,value.find("\""));
                        if(need_replace == ""){
                            value = value.substr(1);
                            continue;
                        }else{
                            for(int j = 0; j < arg_sorted.size(); j++){
                                need_replace = regex_replace(need_replace,regex(arg_sorted[j]),"arg"+to_string(j));
                            }
                            value0 += need_replace;
                        }
                        value = value.substr(value.find("\"")+1);
                    } else if(value.find("\"") != -1 && left_quo){
                        left_quo = false;
                        value0 += "\"" + value.substr(0,value.find("\"")) + "\"";
                        if(value.find("\"") != value.length() - 1){
                            value = value.substr(value.find("\"")+1);
                        } else{
                            value = "";
                        }
                    } else if(!left_quo){
                        need_replace = value;
                        for(int j = 0; j < arg_sorted.size(); j++){
                            need_replace = regex_replace(need_replace,regex(arg_sorted[j]),"arg"+to_string(j));
                        }
                        value0 += need_replace;
                        value = "";
                    }
                }
                value = value0;
            } else{
                for(int j = 0; j < arg_sorted.size(); j++){
                    value = regex_replace(value,regex(arg_sorted[j]),"arg"+to_string(j));
                }
            }

            key = regex_replace(key,regex("[(].*[)]"),"()");
        }

        define_map[key] = value;
        sort(key);
    }
}

string connect_vector(vector<string> vec){
    string ret = "";
    for(int i = 0; i < vec.size(); i++){
        ret += vec[i];
        if(i != vec.size() - 1){
            ret += " ";
        }
    }
    return ret;
}