/*  symtab.h
    
    Arthur Flores, Christian Barroso, Jerald Tomada, Nick Schneider
    masc0855
    Team New Mexico
    prog3
    
    CS530, Spring 2016
    Alan Riggins
*/

#ifndef SYMTAB_H
#define SYMTAB_H

#include "symtab_exception.h"
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>


using namespace std;

class symtab {
    public:
	 // ctor
        // creates a new dictionary structure that stores user
	//defined labels for second pass of assembler
	symtab();
	
	//add label to symbol table
	void add_sym(int, string, string, string);
	
	//get the value for the corresponding label in the parameters
	string get_address(string);
	
	//check if the symbol already exists or not
	bool sym_exists(string);
	
	//edit value when second pass comes around
	void sym_edit_value(string);
	
    private:
        // your variables and private methods go here
	map<string,string> sym_map;
	
	string to_lower(string);
};

#endif
