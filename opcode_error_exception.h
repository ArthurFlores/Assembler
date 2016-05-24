/*  opcode_error_exception.h
    
    Arthur Flores, Christian Barroso, Jerald Tomada, Nick Schneider
    masc0855
    Team New Mexico
    prog2
    
    CS530 Spring 2016
    Alan Riggins
*/   

#ifndef opcodetab_h
#define opcodetab_h
#include <string>

using namespace std;

class opcode_error_exception {

public:
    opcode_error_exception(string s) {
        message = s;        
        }
        
    opcode_error_exception() {
        message = "An error has occurred";
        }
        
    string getMessage() {
        return message;
    }
    
private:
    string message;
};  

#endif  
