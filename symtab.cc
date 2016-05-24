/*  symtab.cc
    
    Arthur Flores, Christian Barroso, Jerald Tomada, Nick Schneider
    masc0855
    Team New Mexico
    prog3
    
    CS530, Spring 2016
    Alan Riggins
*/

#include "symtab.h"

using namespace std;

// ctor
// creates a new dictionary structure that stores user
//defined labels for second pass of assembler
symtab::symtab(){}

//adds a symbol to the map and also checks EQU for the address or label and may wait for 2nd pass
void symtab::add_sym(int loc_counter, string symbol, string opcode, string operand){
	string address;
	if(to_lower(opcode) == "equ") {		//Check if opcode is equ
		if (operand[0] == '$') {	//if operand is hex
			int val;
			string tmp;	
			operand.erase(0,1);
			if(operand.find_first_not_of("0123456789ABCDEF") != string::npos){
				throw symtab_exception("'"+symbol+"' Must be HEX if $ is present in operand");
			}
			sscanf(operand.c_str(),"%x",&val); //transforms hex number to int
			stringstream stream;
			stream << val;
			stream >> tmp;
			address = tmp;
		}
		else if(isdigit(operand[0])) {		//if operand is base 2
			if(operand.find_first_not_of("0123456789") != string::npos){
				throw symtab_exception("'"+symbol+"' Must be int if first field in operand is int");
			}
			address = operand;
		}	
		else	//if a label may or may not wait for second pass
			try {
				address = get_address(operand);
			} catch(symtab_exception symtable_exception) {
				address = operand;
			}
	}
	
	else if(sym_exists(symbol))	//if label already exists
		throw symtab_exception("'" + symbol + "' has already been defined in symtable");
	else{
		stringstream stream;
		string tmp;
		stream << loc_counter;
		stream >> tmp;
		address = tmp;
	}
	sym_map[to_lower(symbol)] = to_lower(address);	//add symbol to the map
}

//get the value for the corresponding label in the parameters
string symtab::get_address(string symbol){
	string value;
	if(!sym_exists(to_lower(symbol)))
		throw symtab_exception("'"+symbol+"' does not exist in symtable");
	else{
		value = sym_map[to_lower(symbol)];
		return value;
	}
}

//checks id the symbol already exists within the map
bool symtab::sym_exists(string sym){
	return (sym_map.find(to_lower(sym)) != sym_map.end());
}

//This will edit address during second pass
void symtab::sym_edit_value(string label){
	if(sym_exists(get_address(to_lower(label)))){
		string value;
		value = get_address(to_lower(label));
		sym_map[to_lower(label)] = get_address(value);
	}
	else
		throw symtab_exception("'"+label+"'error during second pass forward referencing");
}

//private methods

string symtab::to_lower(string symbol) {
	string tmp = symbol;
	transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
	return tmp;
}

