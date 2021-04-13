#include <vector>
#include <string>
#include <initializer_list>

using namespace std;

//// split 'cmd'  by ' ' string and store all parts in vector of string.
vector<string> tokenizer(string cmd){
    vector<string> ans;
    char* tk;
    tk = strtok((char*)cmd.c_str(), " ");
    while(tk != nullptr)
    {
        ans.push_back(tk);
        tk = strtok(nullptr, " ");
    }
    return ans;
}

//// Convert elements inside 'vec' to string
string join(initializer_list<string> vec){
    string ans;
    for(string s: vec)
      ans = ans + s + " ";
    ans.pop_back();
    return ans;
}

//// Convert elements inside 'vec' to string
string joinBySanti(initializer_list<string> vec){
    string ans;
    for(string s: vec)
      ans = ans + s;
    return ans;
}

//// join elements in 'vec' using 'sep'
string split(vector<string> vec, string sep){
    string ans;
    if(!vec.empty())
    {
        ans += vec[0];
        for(int i=1; i<vec.size(); ++i)
            ans += sep + vec[i];
    }
    return ans;
}

string len(string s, int maxlen) // rellena zeros a la izquierda
{
    string zeros(maxlen - to_string(s.length()).length(), '0');
    return zeros + to_string(s.length());
}

string fillZero(string s, int maxlen){
    string zeros(maxlen - s.length(), '0');
    return zeros + s;
}


//// get msg from cmd --- be care sure!
string parserGetMsg(string cmd){
    string msg;
    int utok = 0;
    for(int i=0; i<cmd.size(); ++i)
    {
        if(i==0 && !isspace(cmd[i]))
          utok += 1;
        else if(isspace(cmd[i-1]) && !isspace(cmd[i]))
          utok += 1;

        if(utok >= 3)
          msg.push_back(cmd[i]);
    }
    return msg;
}

//// toma un campo basado en el el valor que obtiene
/**
 * Purpose:
 *  get field from msg using structure.  
 * Params
 *  - pack: msg to process.
 *  - idx: position to get size of field.
 *  - bytes: number of bytes to contain the field in msg.
 * Return a field.
 * NOTE:
 *  - the structure of pack is: <# bytes of field><field value>
 *  - this method implement part of protocols.
 */
string parserGetField(string pack, int& idx, int bytes){
    string sizestr(pack.begin()+idx, 
                   pack.begin()+idx+bytes);
    int size = stoi(sizestr);
    string field(pack.begin()+idx+bytes+1,
                 pack.begin()+idx+bytes+1+size);
    idx += bytes+size+2; // para el prox parseo, actualiza el indice por eso se para por referencia
    return field;
}

/**
 * Purpose:
 *  generate a error message
 * Params:
 *  - errormsg: error message to send
 * return to message 
 */
string errorMsg(string errormsg){
    errormsg = "error: " + errormsg;
    string response = join({"4", len(errormsg, 3), errormsg});
    return response;
}

/**
 * Purpose:
 *  get field in int type from msg using structure of santi protocols.
 * Params
 *  - pack: msg to process.
 *  - idx: position to get size of field.
 *  - bytes: number of bytes to contain the field in msg.
 * Return a field.
 * NOTE:
 *  - extract number to get number in byte of field.
 */
int parserGetFieldByInt(string pack, int& idx, int bytes){
    string sizestr(pack.begin()+idx, 
                   pack.begin()+idx+bytes);
    int size = stoi(sizestr);
    idx += bytes; /// update position of index
    return size;
}

/**
 * Purpose:
 *  get field in string type from msg using structure of santi protocols.
 * Params
 *  - pack: msg to process.
 *  - idx: position to get size of field.
 *  - bytes: number of bytes to contain the field in msg.
 * Return a field.
 * NOTE:
 *  - extract field value from msg.
 */
string parserGetFieldByString(string pack, int& idx, int bytes){
    string sizestr(pack.begin()+idx, 
                   pack.begin()+idx+bytes);
    idx += bytes; /// update position of index
    return sizestr;
}
