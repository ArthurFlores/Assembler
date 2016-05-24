/*  opcodetab.cc
    
    Arthur Flores, Christian Barroso, Jerald Tomada, Nick Schneider
    masc0855
    Team New Mexico
    prog2
    
    CS530, Spring 2016
    Alan Riggins
*/

#include "opcodetab.h"

using namespace std;

// ctor
// creates a new dictionary structure and loads all of the opcodes for 
// the SIC/XE architecture into the table.  Use the STL
// map for this.
opcodetab::opcodetab() {
	string opnames [59] = {"add", "addf", "addr", "and", "clear", "comp", "compf", "compr", "div", 
			       "divf", "divr", "fix", "float", "hio", "j", "jeq", "jgt", "jlt", "jsub",
			       "lda", "ldb", "ldch", "ldf", "ldl", "lds", "ldt", "ldx", "lps", "mul", 
			       "mulf", "mulr", "norm", "or", "rd", "rmo", "rsub", "shiftl", "shiftr", 
			       "sio", "ssk", "sta", "stb", "stch", "stf", "sti", "stl", "sts", "stsw", 
			       "stt", "stx", "sub", "subf", "subr", "svc", "td", "tio", "tix", "tixr", 
			       "wd"};
	string opcodes [59] = {"18", "58", "90", "40", "B4", "28", "88", "A0", "24", "64", "9C", "C4", 
			       "C0", "F4", "3C", "30", "34", "38", "48", "00", "68", "50", "70", "08", 
			       "6C", "74", "04", "D0", "20", "60", "98", "C8", "44", "D8", "AC", "4C", 
			       "A4", "A8", "F0", "EC", "0C", "78", "54", "80", "D4", "14", "7C", "E8", 
			       "84", "10", "1C", "5C", "94", "B0", "E0", "F8", "2C","B8", "DC"};
	int formats [59] = {3, 3, 2, 3, 2, 3, 3, 2, 3, 3, 2, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
			    3, 3, 3, 3, 3, 2, 1, 3, 3, 2, 3, 2, 2, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
			    3, 3, 2, 2, 3, 1, 3, 2, 3};
	opcode_info tmp;
	for (int i = 0; i < TOTAL_OPCODES; i++) {
		tmp.set_info(opcodes[i], formats[i]);
		opmap[opnames[i]] = tmp;
	}
}

//dtor
//opcodetab::~opcodetab(){}

// takes a SIC/XE opcode and returns the machine code 
// equivalent as a two byte string in hexadecimal.
// Example:  get_machine_code("ADD") returns the value 18
// Note that opcodes may be prepended with a '+'.
// throws an opcode_error_exception if the opcode is not 
// found in the table.
string opcodetab::get_machine_code(string code) {
	int plus = 0;
	if (code[0] == '+') {
		code.erase(code.begin());
		plus = 1;
	}
	string tmp = code;
	transform (code.begin(), code.end(), code.begin(), tolower);
	if (code == "")
		throw opcode_error_exception("Invalid: empty string.");
	if (opmap.count(code) == 0)
		throw opcode_error_exception("Invalid opcode: " + tmp);
	if (opmap[code].get_size() != 3 && plus == 1)
		throw opcode_error_exception("Invalid: + is not compatible with opcode " + tmp);
	return opmap[code].get_code();
}
        
// takes a SIC/XE opcode and returns the number of bytes 
// needed to encode the instruction, which is an int in
// the range 1..4.
// NOTE: the opcode must be prepended with a '+' for format 4.
// throws an opcode_error_exception if the opcode is not 
// found in the table.        
int opcodetab::get_instruction_size(string code) {
	int plus = 0;
	if (code[0] == '+') {
		code.erase(code.begin());
		plus = 1;
	}
	string tmp = code;
	transform (code.begin(), code.end(), code.begin(), tolower);
	if (code == "")
		throw opcode_error_exception("Invalid: empty string.");
	if (opmap.count(code) == 0)
		throw opcode_error_exception("Invalid opcode: " + tmp);
	if (opmap[code].get_size() != 3 && plus == 1)
		throw opcode_error_exception("Invalid: + is not compatible with opcode " + tmp);
	if (opmap[code].get_size() == 3 && plus == 1)
		return 4;
	return opmap[code].get_size();
}
