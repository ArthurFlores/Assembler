/*  opcodetab.h
    
    Arthur Flores, Christian Barroso, Jerald Tomada, Nick Schneider
    masc0855
    Team New Mexico
    prog2
    
    CS530, Spring 2016
    Alan Riggins
*/

#ifndef OPCODETAB_H
#define OPCODETAB_H

#include "opcode_error_exception.h"
#include <map>
#include <string>
#include <algorithm>

#define TOTAL_OPCODES 59

using namespace std;

class opcodetab {
    public:
        // ctor
        // creates a new dictionary structure and loads all of the opcodes for 
        // the SIC/XE architecture into the table.  Use the STL
        // map for this.
        opcodetab();
	
	//dtor
	//~opcodetab();
        
        // takes a SIC/XE opcode and returns the machine code 
        // equivalent as a two byte string in hexadecimal.
        // Example:  get_machine_code("ADD") returns the value 18
        // Note that opcodes may be prepended with a '+'.
        // throws an opcode_error_exception if the opcode is not 
        // found in the table.
        string get_machine_code(string);  
        
        // takes a SIC/XE opcode and returns the number of bytes 
        // needed to encode the instruction, which is an int in
        // the range 1..4.
        // NOTE: the opcode must be prepended with a '+' for format 4.
        // throws an opcode_error_exception if the opcode is not 
        // found in the table.        
        int get_instruction_size(string);
                        
    private:
        // your variables and private methods go here
	struct opcode_info {
		string opcode;
		int size;
		
		opcode_info() {
			opcode = "";
			size = 0;
		}
		
		void set_info(string code, int sz) {
			opcode = code;
			size = sz;
		}
		
		string get_code() {
			return opcode;
		}
		
		int get_size() {
			return size;
		}
	};
	
	map<string,opcode_info> opmap;

};

#endif
