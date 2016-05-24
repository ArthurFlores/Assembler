/*  symtab_error_exception.h
    
    Arthur Flores, Christian Barroso, Jerald Tomada, Nick Schneider
    masc0855
    Team New Mexico
    prog3
    
    CS530 Spring 2016
    Alan Riggins
*/   

#ifndef symtab_h
#define symtab_h
#include <string>

using namespace std;

class symtab_exception {

public:
    symtab_exception(string s) {
        message = s;        
        }
        
    symtab_exception() {
        message = "An error has occurred";
        }
        
    string getMessage() {
        return message;
    }
    
private:
    string message;
};  

#endif  
