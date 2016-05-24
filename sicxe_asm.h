/*		
    Arthur Flores, Christian Barroso, Jerald Tomada, Nick Schneider
    masc0855
    Team New Mexico
    prog4
    
    CS530 Spring 2016
    Alan Riggins
*/
#ifndef SICXE_ASM_H
#define SICXE_ASM_H

#include "file_parser.h"
#include "symtab.h"
#include "opcodetab.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <sstream>
#include <cctype>
#include <cstdio>

#define nbit 0x20000
#define ibit 0x10000
#define xbit 0x8000
#define bbit 0x4000
#define pbit 0x2000
#define ebit 0x1000



using namespace std;

class sicxe_asm {
	public:
		// constructor
		sicxe_asm();
		
		// first pass - assign addresses
		void first_pass(int, string);
		
		//Second pass makes machine code
		void second_pass(int, string);
		
	private:
		//uses file_parser's read_file()
		//to parse assembly file
		file_parser parse_asm(int, string);
		
		//writes info about current line in lis file
		void write_in_lis(ofstream&, file_parser, unsigned int, unsigned int, unsigned int);
		
		void add_to_lis(string, unsigned int);
		
		//populates symtab and print info to .lis file using write_in_lis()
		void populate(file_parser, ofstream&, string);
		
		//makes a string all lower-case
		string to_lower(string);
		
		//returns 1 if string given is a label, 0 otherwise
		bool is_label(string);
		
		//returns 1 if string given is hex, 0 otherwise
		bool is_hex(string);
		
		//Gets the value for the registers for format 2
		int *get_reg_num(string, unsigned int, int);
		
		//formats the Machine Code with no leading zeros 
		string formatHexNumber(int , int );
		
		vector<int> nolabel; //this is for directives with no labels
		vector<string> lis_contents;
		opcodetab opcodes;
		symtab symbols;
		symtab equ_symbols;
		bool base_set;
		int base_row;
		string lisfilename;
		int reg_machine[2];
		
};

#endif
