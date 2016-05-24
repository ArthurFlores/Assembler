/*		
    Arthur Flores, Christian Barroso , Jerald Tomada, Nick Schneider
    masc0855
    Team New Mexico
    prog4
    
    CS530 Spring 2016
    Alan Riggins
*/

#include "sicxe_asm.h"

using namespace std;

sicxe_asm::sicxe_asm(){}

void sicxe_asm::first_pass(int argc, string argv) {
	
	file_parser parser = parse_asm(argc, argv);
	
	lisfilename = argv.replace(argv.length()-4, string::npos, ".lis");
    	ofstream lisfile (lisfilename.c_str());
   	lisfile << setfill(' ') << setw(24);
	lisfile << "**" << lisfilename << "**\n";
    	lisfile << "Line#" << setfill(' ') << setw(8);
	lisfile << "Address" << setfill(' ') << setw(12);
	lisfile << "Label" << setfill(' ') << setw(10);
	lisfile << "Opcode" << setfill(' ') << setw(10);
	lisfile << "Operand" << setfill(' ') << setw(24);
	lisfile << "Machine Code\n";
	lisfile << "=====" << setfill(' ') << setw(8);
	lisfile << "=======" << setfill(' ') << setw(12);
	lisfile << "=====" << setfill(' ') << setw(10);
	lisfile << "======" << setfill(' ') << setw(10);
	lisfile << "=======" << setfill(' ') << setw(24);
	lisfile << "============\n";
	
    	populate(parser, lisfile, lisfilename);
	
	lisfile.close();
}

void sicxe_asm::second_pass(int argc, string argv) {
	file_parser parser = parse_asm(argc, argv);
	
	unsigned int index = 1;
	unsigned int nolabel_count = 0;
	int destination, source;
	int row, instruction, format, offset, tmp;
	int bitn, biti, bitx, bitb, bitp, bite;
	string opcode_num, lis_line, mach_code, label_oper;
	ifstream lis_file, reg_num;
	stringstream stream, ss;
	bool symbol_in_oper;
	string equ;
	
	lisfilename = argv.replace(argv.length()-4, string::npos, ".lis");
	lis_file.open(lisfilename.c_str(), ios::in);
	while (!lis_file.eof()) {
		getline(lis_file, lis_line);
		lis_contents.push_back(lis_line);
	}
	
	for (row = 0; row < parser.size(); row++) {
		bitn = 0, biti = 0, bitx = 0, bitb = 0, bitp = 0, bite = 0;
		destination = 0 , source = 0;
		instruction = 0;
		tmp = 0;
		equ = "";
		
		//if no opcode then it generates no machine code so go to next row
		if (parser.get_token(row, 1).empty()) {
			index++;
			nolabel_count++;
			continue;
		}
		else {
			try {
				//try getting size of opcode
				format = opcodes.get_instruction_size(parser.get_token(row,1));
				
				//if format is 1...
				if (format == 1) {
					//...and there is an operand
					if (!(parser.get_token(row,2).empty())) {
						cout << "Error [Line " << dec << index << "] Format 1 operator should not have an operand" << endl;
						remove(lisfilename.c_str());
						exit(1);
					}
					//if all goes well, write instruction (opcode) to lis
					opcode_num = opcodes.get_machine_code(parser.get_token(row,1));
					add_to_lis(opcode_num, row);
				}
				//if format is 2...
				else if (format == 2) {
					int reg1 = 0;
					int reg2 = 0;
					int *reg_point;
					if (parser.get_token(row,2).empty()) {
						cout << "Error [Line " << dec << index << "] Format 2 operator should have an operand" << endl;
						remove(lisfilename.c_str());
						exit(1);
					}
					// ... convert the opcode string to an int
					opcode_num = opcodes.get_machine_code(parser.get_token(row,1));
					ss.clear();
					ss << std::hex << opcode_num;
					ss >> tmp;
					//if there is immediate used with index error
					if(parser.get_token(row,2).find_first_of(",") != std::string::npos &&
					parser.get_token(row,2).find_first_of("#") != std::string::npos){
						cout << "Error [Line " << dec << index << "] invalid operand, immediate and index cannot be used together" << endl;
						remove(lisfilename.c_str());
						exit(1);
					}
					
					//get machine code for only ONE Register...for exceptions TIXR,CLEAR
					if(opcode_num == "B8" || opcode_num == "B4"){
						if(parser.get_token(row,2).size() > 1){
							if(parser.get_token(row,2).size() > 2){
								cout << "Error [Line " << dec << index << "] invalid operand for TIXR or CLEAR" << endl;
								remove(lisfilename.c_str());
								exit(1);
							}
							else if((parser.get_token(row,2)[0] == 's' &&
							parser.get_token(row,2)[1] == 'w') || 
							(parser.get_token(row,2)[0] == 'p' &&
							parser.get_token(row,2)[1] == 'c')){
							}
							else{
								cout << "Error [Line " << dec << index << "] invalid operand for TIXR or CLEAR" << endl;
								remove(lisfilename.c_str());
								exit(1);
							}
							
						}
						reg_point = get_reg_num(parser.get_token(row, 2), index, 1);
						reg1 = *reg_point;
						reg2 = 0;
					}
					//...SVC get number
					else if(opcode_num == "B0"){
						if(parser.get_token(row,2)[0] == '#'){
							cout << "Error [Line " << dec << index << "] invalid operand for SVC no immediate used" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
						reg_point = get_reg_num(parser.get_token(row, 2), index, 3);
						reg1 = *reg_point;
						reg2 = 0;
					}
					//... for SHIFTL and SHIFTR
					else if(opcode_num == "A4" || opcode_num == "A8"){
						reg_point = get_reg_num(parser.get_token(row, 2), index, 4);
						reg1 = *reg_point;
						reg2 = *(reg_point+1);
					}
					//...get machine code for TWO registers...
					else {
						if(parser.get_token(row, 2).size() > 5 || parser.get_token(row, 2).size() < 3){
							cout << "Error [Line " << dec << index << "] invalid operand for format2" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
						reg_point = get_reg_num(parser.get_token(row, 2), index, 2);
						reg1 = *reg_point;
						reg2 = *(reg_point+1);
					}
					//...get machine code for opcode and combine with code for regs...  
					instruction = tmp << 8;
					instruction |= reg1 << 4;
					instruction |= reg2;
					mach_code = formatHexNumber(instruction, 4);
					
					//...and write it to lis
					add_to_lis(mach_code, row);
					
				}
				//if format 3
				else if (format == 3) {
					// ... convert opcode string to an int & add to instruction
					opcode_num = opcodes.get_machine_code(parser.get_token(row,1));
					ss.clear();
					ss << std::hex << opcode_num;
					ss >> tmp;
					instruction = tmp << 16;
					//...for RSUB
					if(opcode_num[0] == '4' && opcode_num[1] == 'C') {
						if(parser.get_token(row,2).size() > 0) {
							cout << "Error [Line " << dec << index << "] there cannot be a operand with opcode RSUB" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
						//set n and i for RSUB
						instruction |= nbit;
						instruction |= ibit;
						
						//write it all to lis and update loccounter
						mach_code = formatHexNumber(instruction, 6);
						add_to_lis(mach_code, row);
						index++;
						nolabel_count++;
						continue;
					}
					//...if operand field empty 
					if (parser.get_token(row,2).empty()) {
						cout << "Error [Line " << dec << index << "] Format 3 operator should have an operand except RSUB" << endl;
						remove(lisfilename.c_str());
						exit(1);
					}
					//... check operands
					symbol_in_oper = 0;
					if(parser.get_token(row,2)[0] == '@') {
						bitn = 1;
						symbol_in_oper = 1;
					}
					if(parser.get_token(row,2)[0] == '#') {
						biti = 1;
						symbol_in_oper = 0;
					}
					if((parser.get_token(row,2).find_first_of(",")) != std::string::npos) {
						bitx = 1;
						symbol_in_oper = 1;
					}
					
					if((bitn == 1 && bitx == 1) || (biti == 1 && bitx == 1)){
						cout << "Error [Line " << dec << index << "] operand cannot use indirect or immediate with the index register." << endl;
						remove(lisfilename.c_str());
						exit(1);
					}
					
					//if indirect, index, or immeditae with a label - try pc method
					//if immediate, #, make sure there's a label and grab it
					if (biti == 1 && bitn == 0 && bitx == 0 &&
						is_label(parser.get_token(row,2).substr(1,string::npos)) == 1) {
						label_oper = parser.get_token(row,2).erase(0,1);
						symbol_in_oper = 1;
						
					}
					//if indirect, grab what's after @
					else if (bitn == 1 && biti == 0 && bitx == 0) {
						label_oper = parser.get_token(row,2).erase(0,1);
						try{
							symbols.get_address(label_oper);
						}
						catch(symtab_exception e){
							symbol_in_oper = 0;
						}
					}
					//if index, find the label and grab it
					else if (bitx == 1 && (biti == 0 && bitn == 0)) {
						//if x comes before ','
						if (to_lower(parser.get_token(row,2))[0] == 'x') {
							label_oper = parser.get_token(row,2).substr(2,string::npos);
						}
						//if x comes after the ','
						else if (to_lower(parser.get_token(row,2))[parser.get_token(row,2).size()-1] == 'x') {
							label_oper = parser.get_token(row,2).substr(0,parser.get_token(row,2).size()-2);
						}
						//if there is no x
						else {
							cout << "Error [Line " << dec << index << "] there must be an X when using indexing" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
						
						try{
							symbols.get_address(label_oper);
						}
						catch(symtab_exception e){
							symbol_in_oper = 0;
						}
					}
					
					//if there is a label not preceeded by a # or @
					else if (is_label(parser.get_token(row,2)) == 1) {
						label_oper = parser.get_token(row,2);
						symbol_in_oper = 1;
					}
					
					try {
						equ_symbols.get_address(label_oper);
						symbol_in_oper = 0;
						equ = equ_symbols.get_address(label_oper);
						
					}
					catch(symtab_exception e){}
					
					//if there is a label in the operand
					if (symbol_in_oper == 1) {
						//find source
						//if label column isn't empty
						if (!(parser.get_token(row,0).empty())) {
							ss.clear();
							ss << dec << symbols.get_address(parser.get_token(row,0));
							ss >> source;
							ss.clear();
						}
						//if label column is empty
						else {
							ss.clear();
							ss << std::hex << nolabel[nolabel_count];
							ss >> source;
							ss.clear();						
						}
						//get destination
						try {
							//try to get it from symbtab
							ss.clear();
							ss << dec << symbols.get_address(label_oper);
							ss >> destination;
							ss.clear();	
						}
						//if exception is thrown, show error message
						catch (symtab_exception e) {
							cout << "Error [Line " << dec << index << "] symbol in operand never defined" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
						
						
						//if all goes well, perform calculation for pc relative
						offset = destination - (source+3);
						
						//if offset doesn't fit in pc relative
						if (offset > 2047 || offset < -2048) {
							bitp = 0;
							//if base is not set, throw error message
							if (base_set == 0) {
								cout << "Error [Line " << dec << index << "] instruction too big for PC and BASE was not set" << endl;
								remove(lisfilename.c_str());
								exit(1);
							}
							//base is set, so grab address from operand in base's row
							//if the base operand is a label
							if (is_label(parser.get_token(base_row,2))) {
								//try getting it's address from symtab
								try {
									ss.clear();
									ss << dec << symbols.get_address(parser.get_token(base_row,2));
									ss >> source;
								}
								//if exception is thrown, throw error message
								catch (symtab_exception e) {
									cout << "Error [Line " << base_row << "] symbol in operand for BASE never defined" << endl;
									remove(lisfilename.c_str());
									exit(1);
								}
							}
							//if base operand not a label, must be hex
							else {
								ss.clear();
								ss << hex << parser.get_token(base_row,2).erase(0,1);
								ss >> source;
								
								if (source > 2047) {
									cout << "Error [Line " << dec << index << "] BASE is set past the limit of 2047" << endl;
									remove(lisfilename.c_str());
									exit(1);
								}
							}	
							//if all goes well, calculate offset for BASE relative
							//offset = (Desired Destination) - (Base)
							offset = destination - source;
					
							//if offset is too big for BASE relative, throw error message
							if (offset < 0 || offset > 4095) {
								cout << "Error [Line " << dec << index << "] instruction is out of range for BASE. Use format 4" << endl;
								remove(lisfilename.c_str());
								exit(1);
							}
							//if offset fits BASE relative
							else
								bitb = 1;
							
						}
						//if offset fits PC relative
						else {
							//this removes the leading F's from the int.
							if(offset < 0)
								offset &= 0x00000FFF;
								
							bitp = 1;
							bitb = 0;
						}
					}
					//if there is no label, no bitn set, no bitx set, and no biti set
					else {
						//if immediate constant so #C you can only have numbers -2048 thru 2047 for format3
						if(biti == 1){
							if(parser.get_token(row,2)[1] == '$'){
								ss.clear();
								ss << hex << parser.get_token(row,2).erase(0,2);
								ss >> offset;
							}
							else{
								ss.clear();
								ss << dec << parser.get_token(row,2).erase(0,1);
								ss >> offset;
							}
							if(offset < -2048 || offset > 2047){
								cout << "Error [Line " << dec << index << "] operand can only have numbers -2048 thru 2047 with #C" << endl;
								remove(lisfilename.c_str());
								exit(1);
							}
							if(offset < 0)
								offset &= 0x00000FFF;
						}
						//if constant so C you can only have numbers 0 thru 4095 for format3
						else{
							string tmp = parser.get_token(row,2);
							if(bitn == 1)
								tmp = parser.get_token(row,2).erase(0,1);
							if(bitx == 1)
								tmp = label_oper;
							if(equ != "")
								tmp = equ;
							if(tmp[0] == '$'){
								ss.clear();
								ss << hex << tmp.erase(0,1);
								ss >> offset;
							}
							else{
								ss.clear();
								ss << dec << tmp;
								ss >> offset;
							}
							if(offset < 0 || offset > 4095){
								cout << "Error [Line " << dec << index << "] operand can only have numbers 0 thru 4095 with C constant" << endl;
								remove(lisfilename.c_str());
								exit(1);
							}
						}
						
					}
					
					// ... add nixbpe bits
					//to set n bit
					if( bitn == 1 || (bitn == 0 && biti == 0))
						instruction |= nbit;
					//to set i bit
					if( biti == 1 || (bitn == 0 && biti == 0))
						instruction |= ibit;
					//to set x bit
					if( bitx == 1 && bitn == 0 && biti == 0)
						instruction |= xbit;
					//to set b bit
					if( bitb == 1 && bitp ==0)
						instruction |= bbit;
					//to set p bit
					if( bitp == 1 && bitb ==0)
						instruction |= pbit;
						
					//put offset in instruction
					instruction |= offset;
					//don't need to set e bit since its format 3
					
					//...write it to lis file
					mach_code = formatHexNumber(instruction, 6);
					add_to_lis(mach_code, row);
					
				}
				//if format 4
				else if (format == 4){
					
					// ... convert opcode string to an int & add to instruction
					opcode_num = opcodes.get_machine_code(parser.get_token(row,1));
					ss.clear();
					ss << std::hex << opcode_num;
					ss >> tmp;
					instruction = tmp << 16;
					//CHECK RSUB
					if(opcode_num[0] == '4' && opcode_num[1] == 'C'){
						if(parser.get_token(row,2).size() > 0){
							cout << "Error [Line " << dec << index << "] there cannot be a operand with opcode RSUB" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
						//set n and i for RSUB
						instruction |= nbit;
						instruction |= ibit;
						instruction = instruction << 8;
						//write it all to lis and update loccounter
						mach_code = formatHexNumber(instruction, 8);
						add_to_lis(mach_code, row);
						index++;
						nolabel_count++;
						continue;
					}
					//...if operand is empty
					if (parser.get_token(row,2).empty()) {
						cout << "Error [Line " << dec << index << "] Format 4 operator should have an operand except RSUB" << endl;
						remove(lisfilename.c_str());
						exit(1);
					}
					//... check operands
					symbol_in_oper = 0;
					if(parser.get_token(row,2)[0] == '@') {
						bitn = 1;
						symbol_in_oper = 1;
					}
					if(parser.get_token(row,2)[0] == '#') {
						biti = 1;
						symbol_in_oper = 0;
					}
					if((parser.get_token(row,2).find_first_of(",")) != std::string::npos) {
						bitx = 1;
						symbol_in_oper = 1;
					}
					
					if((bitn == 1 && bitx == 1) || (biti == 1 && bitx == 1)){
						cout << "Error [Line " << dec << index << "] operand cannot use indirect or immediate with the index register." << endl;
						remove(lisfilename.c_str());
						exit(1);
					}
					// ... add nixbpe bits
					//to set n bit
					if( bitn == 1 || (bitn == 0 && biti == 0))
						instruction |= nbit;
					//to set i bit
					if( biti == 1 || (bitn == 0 && biti == 0))
						instruction |= ibit;
					//to set x bit
					if( bitx == 1 && bitn == 0 && biti == 0)
						instruction |= xbit;
					//Always set e bit for format 4
					instruction |= ebit;
					//move bits over so its a 4 byte instruction
					instruction = instruction << 8;
					
					//if indirect, index, or immeditae with a label - try pc method
					//if immediate, #, make sure there's a label and grab it
					if (biti == 1 && bitn == 0 && bitx == 0 &&
						is_label(parser.get_token(row,2).substr(1,string::npos)) == 1) {
						label_oper = parser.get_token(row,2).erase(0,1);
						symbol_in_oper = 1;
					}
					//if indirect, grab what's after @
					else if (bitn == 1 && biti == 0 && bitx == 0) {
						label_oper = parser.get_token(row,2).erase(0,1);
						try{
							symbols.get_address(label_oper);
						}
						catch(symtab_exception e){
							symbol_in_oper = 0;
						}
					}
					//if index, find the label and grab it
					else if (bitx == 1 && (biti == 0 && bitn == 0)) {
						//if x comes before ','
						if (to_lower(parser.get_token(row,2))[0] == 'x') {
							label_oper = parser.get_token(row,2).substr(2,string::npos);
						}
						//if x comes after the ','
						else if (to_lower(parser.get_token(row,2))[parser.get_token(row,2).size()-1] == 'x') {
							label_oper = parser.get_token(row,2).substr(0,parser.get_token(row,2).size()-2);
						}
						//if there is no x
						else {
							cout << "Error [Line " << dec << index << "] there must be an X when using indexing" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
						try{
							symbols.get_address(label_oper);
						}
						catch(symtab_exception e){
							symbol_in_oper = 0;
						}
					}
					//if there is a label not preceeded by a # or @
					else if (is_label(parser.get_token(row,2)) == 1) {
						label_oper = parser.get_token(row,2);
						symbol_in_oper = 1;
					}
					//if there is a label in the operand
					if (symbol_in_oper == 1) {
					
						//get destination
						try {
							//try to get it from symbtab
							ss.clear();
							ss << dec << symbols.get_address(label_oper);
							ss >> destination;
							ss.clear();
						}
						//if exception is thrown, show error message
						catch (symtab_exception e) {
							cout << "Error [Line " << dec << index << "] symbol in operand never defined" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
						
						//if all goes well, check if destination is greater than 20 bit size
						if (destination > 1048575 || destination < 0) {
							cout << "Error [Line " << dec << index << "] Target address cannot fit in 20 bits" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
					}
					// else there is no label in the operand, biti isnt set, or bitn or bit x
					else {
						//if immediate constant so #C you can only have numbers -524288 thru 524287 for format4
						if(biti == 1){
							if(parser.get_token(row,2)[1] == '$'){
								ss.clear();
								ss << hex << parser.get_token(row,2).erase(0,2);
								ss >> destination;
							}
							else{
								ss.clear();
								ss << dec << parser.get_token(row,2).erase(0,1);
								ss >> destination;
							}
							if(destination < -524288 || destination > 524287){
								cout << "Error [Line " << dec << index << "] operand can only have numbers -524288 thru 524287 with #C in format 4" << endl;
								remove(lisfilename.c_str());
								exit(1);
							}
							if(destination < 0)
								destination &= 0x000FFFFF;
						}
						//if constant so C you can only have numbers 0 thru 1048575 for format4
						else{
							string tmp = parser.get_token(row,2);
							if(bitn == 1)
								tmp = parser.get_token(row,2).erase(0,1);
							if(bitx == 1)
								tmp = label_oper;
							if(tmp[0] == '$'){
								ss.clear();
								ss << hex << tmp.erase(0,1);
								ss >> destination;
							}
							else{
								ss.clear();
								ss << dec << tmp;
								ss >> destination;
							}
							if (destination < 0 || destination > 1048575) {
								cout << "Error [Line " << dec << index << "] operand can only have numbers 0 and 1048575 with C constant in format 4" << endl;
								remove(lisfilename.c_str());
								exit(1);
							}
						}
					}
					
					instruction |= destination;
					
					//...write it to lis file
					mach_code = formatHexNumber(instruction, 8);
					add_to_lis(mach_code, row);
					// ... add nixbpe dont set B or P and always set E
				}
				
			}	
			catch (opcode_error_exception e) {
				if(to_lower(parser.get_token(row, 1)) == "word") {
					//if in hex
					if (parser.get_token(row,2).empty()) {
						cout << "Error [Line " << dec << index << "] WORD operator should have an operand" << endl;
						remove(lisfilename.c_str());
						exit(1);
					}
					if(parser.get_token(row,2)[0] == '$') {
						ss.clear();
						ss << hex << parser.get_token(row,2).erase(0,1);
						ss >> instruction;
						if(instruction > 16777215) {
							cout << "Error [Line " << dec << index << "] operand can only have numbers to 16777215 with word" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
					}
					else {
					//convert string to int
						stringstream s;
						s << dec << parser.get_token(row,2);
						s >> instruction; 
						if(instruction > 16777215){
							cout << "Error [Line " << dec << index << "] operand can only have numbers to 16777215 with word" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
					}
					//print in lis file
					mach_code = formatHexNumber(instruction, 6);
					add_to_lis(mach_code, row);
				}
				else if(to_lower(parser.get_token(row, 1)) == "byte") {
						if (parser.get_token(row,2).empty()) {
							cout << "Error [Line " << dec << index << "] BYTE operator should have an operand" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
						if(parser.get_token(row,2)[0] == '$') {
							ss.clear();
							ss << hex << parser.get_token(row,2).erase(0,1);
							ss >> instruction;
							if(instruction > 255) {
								cout << "Error [Line " << dec << index << "] operand can only have numbers to 255 with BYTE" << endl;
								remove(lisfilename.c_str());
								exit(1);
							}
							mach_code = formatHexNumber(instruction, 2);
							add_to_lis(mach_code, row);
						}
						//if operand is C'' or X''
						else if (parser.get_token(row,2)[1] == '\'' && parser.get_token(row,2)[parser.get_token(row,2).size()-1] == '\'') {
							//if it's hex
							if (to_lower(parser.get_token(row,2))[0] == 'x') {
								string tmpstring = parser.get_token(row,2).erase(0,2);
								tmpstring.erase(tmpstring.size()-1,tmpstring.size());
								
								//place in machine code
								transform(tmpstring.begin(), tmpstring.end(), tmpstring.begin(), ::toupper);
								add_to_lis(tmpstring, row);
								
							}
							//if it's a character constant
							else {
								string tmpstring = parser.get_token(row,2).erase(0,2);
								tmpstring.erase(tmpstring.size()-1,tmpstring.size());
								
								int tmp;
								string tmp_part;
								string tmpfinal;
								stringstream sstream;
								for(unsigned k=0; k < tmpstring.size();k++){
									tmp_part = "";
									tmp = tmpstring[k];
									sstream.clear();
									sstream << hex << tmp;
									sstream >> tmp_part;
									tmpfinal += tmp_part;
								}
									
								//write it all to lis and update loccounter
								transform(tmpfinal.begin(), tmpfinal.end(), tmpfinal.begin(), ::toupper);
								add_to_lis(tmpfinal, row);
									
							}
						}
						else {
						//convert string to int
							stringstream s;
							s << dec << parser.get_token(row,2);
							s >> instruction; 
							if(instruction > 255) {
								cout << "Error [Line " << dec << index << "] operand can only have numbers to 255 with word" << endl;
								remove(lisfilename.c_str());
								exit(1);
							}
							//write to lis
							mach_code = formatHexNumber(instruction, 2);
							add_to_lis(mach_code, row);
						}
				}
				else {
					//put an empty string in machine code spot since 
					//assembly directive, only put for word and byte
					if(to_lower(parser.get_token(row, 1)) == "resb" ||
					to_lower(parser.get_token(row, 1)) == "resw"){
						if(parser.get_token(row,2).empty()){
							cout << "Error [Line " << dec << index << "] RESW or RESB operator should have an operand" << endl;
							remove(lisfilename.c_str());
							exit(1);
						}
					}
				
				}
			}
		}
		
		index++;
		nolabel_count++;
	
	}
	
	lis_file.close();
	
	ofstream lisfile (lisfilename.c_str());
	//lis_file.open(lisfilename.c_str(), ios::trunc|ios::out);
	for (unsigned int i = 0; i < lis_contents.size(); i++) {
		lisfile << lis_contents[i] << endl;
	}
}

/***************************PRIVATE METHODS BEGIN******************************/
file_parser sicxe_asm::parse_asm(int argc, string argv) {
    	string filename = argv;
	string extension = filename.substr(filename.size()-4, string::npos);
	
	if (extension != ".asm") {
		cout << "Error, your file must have an extension of .asm" << endl;
		exit(1);
	}
	
	try {
		file_parser parser(filename);
   		parser.read_file();
		return parser;
	}
	
	catch(file_parse_exception excpt) {
        	cout << "**Sorry, error " + excpt.getMessage() << endl;
		exit(1);
	}
}

void sicxe_asm::write_in_lis(ofstream& file, file_parser parser, unsigned int index, 
				  	 unsigned int loccounter, unsigned int row) {
	file << setw(5) << dec << index;
	file << "   " << setfill('0') << setw(5) << hex << uppercase << loccounter << nouppercase;
	file << setfill(' ');	// return fill character to blanks
	file << setw(12) << parser.get_token(row, 0);
	file << setw(10) << parser.get_token(row, 1);
	file << "    " << setw(20) << left << parser.get_token(row, 2) << right <<endl;
}

void sicxe_asm::add_to_lis(string instruction, unsigned int row) {
	lis_contents[row+3].insert(58, "   " + instruction);
}

void sicxe_asm::populate(file_parser parser, ofstream& file, string lis) {
	stringstream stream;
	unsigned int index = 1;
	unsigned int loccounter = 0;
	int row;
	unsigned int tmp, opsize;
	string curr_op;
	base_set = 0;
	base_row = 0;
	int start_set = 0;
	int end_set = 0;
	
	for (row = 0; row < parser.size(); row++) {
	
		// check the current line if there is anything in the opcode field
		if (parser.get_token(row, 1).empty()) {
		
			// if none, and there is an operand, cout an error and exit
			if (!(parser.get_token(row, 2).empty())) {
				cout << "Error [Line " << index << "]: "
				     << "operand exists with no opcode." << endl;
				remove(lis.c_str());
				exit(1);
			}
			// if there's only a label
			else if (!(parser.get_token(row, 0).empty())) {
				//throw error if there is no START diretive
				if (start_set == 0) {
				cout << "Error [Line " << index << "]: START directive must be first instruction" << endl;
				remove(lis.c_str());
				exit(1);
				}
				
				symbols.add_sym(loccounter, parser.get_token(row,0), "", "");
				write_in_lis(file, parser, index, loccounter, row);
				nolabel.push_back(loccounter);
			}
			// if none, and there is nothing else on that line, except a comment, then
			else {
				// write in lis an empty line
				write_in_lis(file, parser, index, loccounter, row);
				nolabel.push_back(loccounter);
			}
		}
		
		// if opcode field exists
		else {
			// try getting instruction size from opcodetab
			try {
				opsize = opcodes.get_instruction_size(parser.get_token(row,1));
				
				//throw error if no START directive
				if (start_set == 0) {
					cout << "Error [Line " << index << "]: START directive must be first instruction" << endl;
					remove(lis.c_str());
					exit(1);
				}
				
				// if it returns a valid size / no exception error, then
				// if label exists, enter label into symtab as key with loccounter as its value
				if (!(parser.get_token(row, 0).empty())) {
					symbols.add_sym(loccounter, parser.get_token(row,0), parser.get_token(row,1), parser.get_token(row,2));
					
					// write into lis file
					write_in_lis(file, parser, index, loccounter, row);
					nolabel.push_back(loccounter);
					loccounter += opsize;
				}
				//if label doesn't exist
				else {
					// write into lis file
					write_in_lis(file, parser, index, loccounter, row);
					nolabel.push_back(loccounter);
					loccounter += opsize;
				}
				
			}
					
			// else if it does not return a valid size, it could be an assembler directive
			catch (opcode_error_exception e) {
				//if the directive is START...
				if (to_lower(parser.get_token(row, 1)) == "start") {
					start_set = 1;
					//...and doesn't have a label... 
					if (parser.get_token(row,0).empty()) {
						cout << "Error [Line " << index << "]: "
						     << "START must have a label" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//...or a hex operand or a 0
					else if (is_hex(parser.get_token(row,2)) == 0
						 && parser.get_token(row,2) != "0") {
						cout << "Error [Line " << index << "]: "
						     << "Operand to START directive must either be a hex value or 0" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//START has everything it needs, so we write it to .lis and update loccounter
					else {
						write_in_lis(file, parser, index, loccounter, row);
						nolabel.push_back(loccounter);
						stream << hex << parser.get_token(row, 2).erase(0,1);
						stream >> loccounter;
					}
				}
				
				//if the directive is END...
				else if (to_lower(parser.get_token(row, 1)) == "end") {
					end_set = 1;
					//throw error if there is no START diretive
					if (start_set == 0) {
						cout << "Error [Line " << index << "]: START directive must be first instruction" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//...doesn't have a label operand (if there is an operand)
					if (!(parser.get_token(row,2).empty()) &&
					    is_label(parser.get_token(row,2)) == 0) {
						cout << "Error [Line " << index << "]: "
						     << "Invalid operand for END directive" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//END has everything it needs
					else {
						
						//if label column is not empty
						if (!(parser.get_token(row,0).empty())) {
							symbols.add_sym(loccounter, parser.get_token(row,0), "", "");
							nolabel.push_back(loccounter);
						}
						//if label column is empty
						else {
							nolabel.push_back(loccounter);
						}
						//write into lis
						write_in_lis(file, parser, index, loccounter, row);
					}
				}
				
				//if the directive is EQU...
				else if (to_lower(parser.get_token(row, 1)) == "equ") {
					//throw error if there is no START diretive
					if (start_set == 0) {
						cout << "Error [Line " << index << "]: START directive must be first instruction" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//...and doesn't have a label
					if (parser.get_token(row,0).empty()) {
						cout << "Error [Line " << index << "]: "
						     << "EQU must have a label" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//EQU has a label
					else {
						//try adding label to symbol table
						try {
							symbols.add_sym(loccounter, parser.get_token(row,0), 
									parser.get_token(row,1), parser.get_token(row,2));
							equ_symbols.add_sym(loccounter, parser.get_token(row,0), 
									parser.get_token(row,1), parser.get_token(row,2));
							write_in_lis(file, parser, index, loccounter, row);
							nolabel.push_back(loccounter);
						} 
						//if symtab throws an exception
						catch (symtab_exception e) {
							cout << "Error [Line " << index << "]: "
							     << e.getMessage() << endl;
							remove(lis.c_str());
							exit(1);
						}
					}
				}
				//if the directive is BASE...
				else if(to_lower(parser.get_token(row, 1)) == "base") {
					//throw error if there is no START diretive
					if (start_set == 0) {
						cout << "Error [Line " << index << "]: START directive must be first instruction" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//...and doesnt have a label or a hex value in operand
					if(is_label(parser.get_token(row,2)) == 0 &&
					   is_hex(parser.get_token(row,2)) == 0) {
						cout<< "Error [Line " << index << "]: "
						    << "BASE directive must have either a label or hex value in operand" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//BASE has everything it needs
					else {
						//if label column isn't empty
						if (!(parser.get_token(row,0).empty())) {
							try {
								symbols.add_sym(loccounter, parser.get_token(row,0), "", "");
								nolabel.push_back(loccounter);
							}
							catch(symtab_exception e) {
								cout << "Error [Line " << index << "]: "
								     << e.getMessage() << endl;
								remove(lis.c_str());
								exit(1);
							}
						}
						//if label column is empty
						else
							nolabel.push_back(loccounter);
						//write to .lis
						write_in_lis(file, parser, index, loccounter, row);
						base_set = 1;
						base_row = row;
					}
				}
				//if the directive is NOBASE...
				else if (to_lower(parser.get_token(row, 1)) == "nobase") {
					//throw error if there is no START diretive
					if (start_set == 0) {
						cout << "Error [Line " << index << "]: START directive must be first instruction" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//...there should be no operand
					if (!(parser.get_token(row,2).empty())) {
						cout << "Error [Line " << index << "]: "
						     << "NOBASE directive should not have an operand" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//NOBASE has everything it needs
					else {
						//if label column is not empty
						if (!(parser.get_token(row,0).empty())) {
							symbols.add_sym(loccounter, parser.get_token(row,0), "", "");
							nolabel.push_back(loccounter);
						}
						//if label column is empty
						else
							nolabel.push_back(loccounter);
						//write to .lis
						write_in_lis(file, parser, index, loccounter, row);
						base_set = 0;
						base_row = 0;
					}
				}	
				//if the directive is RESW...
				else if (to_lower(parser.get_token(row, 1)) == "resw") {
					//throw error if there is no START diretive
					if (start_set == 0) {
						cout << "Error [Line " << index << "]: START directive must be first instruction" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//...and the operand is neither dec or a label
					if (parser.get_token(row,2).find_first_not_of("0123456789") != string::npos &&
					    is_label(parser.get_token(row,2)) == 0) {
						cout << "Error [Line " << index << "]: "
						     << "RESW must have either a decimal number or a label as operand" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//RESW has everything it needs
					else {
						//if label column is not empty
						if (!(parser.get_token(row,0).empty())) {
							//we add it to the symbol table
							symbols.add_sym(loccounter,parser.get_token(row,0),"","");
							nolabel.push_back(loccounter);
						}
						//if label column is empty
						else
							nolabel.push_back(loccounter);
							
						//now we write it all to .lis and update loccounter
						write_in_lis(file, parser, index, loccounter, row);
						string toktmp = parser.get_token(row, 2);
						tmp = atoi(toktmp.c_str());
						loccounter += 3*tmp;
						
					}
				}
				//if directive is RESB...
				else if (to_lower(parser.get_token(row, 1)) == "resb") {
					//throw error if there is no START diretive
					if (start_set == 0) {
						cout << "Error [Line " << index << "]: START directive must be first instruction" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//...and the operand is neither not dec or a label
					if (parser.get_token(row,2).find_first_not_of("0123456789") != string::npos &&
					    is_label(parser.get_token(row,2)) == 0) {
						cout << "Error [Line " << index << "]: "
						     << "RESB must have either a decimal number or a label as operand" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//RESB has everything it needs
					else {
						//if label column is not empty
						if (!(parser.get_token(row,0).empty())) {
							//we add it to the symbol table
							symbols.add_sym(loccounter,parser.get_token(row,0),"","");
							nolabel.push_back(loccounter);
						}
						//if label column is empty
						else
							nolabel.push_back(loccounter);
							
						//now we write it all to .lis and update loccounter
						write_in_lis(file, parser, index, loccounter, row);
						//stream << dec << parser.get_token(row, 2);
						//stream >> tmp;
						string toktmp=parser.get_token(row,2);
						tmp = atoi(toktmp.c_str());
						loccounter += tmp;
					}
				}
				//if directive is BYTE...
				else if(to_lower(parser.get_token(row, 1)) == "byte") {
					//throw error if there is no START diretive
					if (start_set == 0) {
						cout << "Error [Line " << index << "]: START directive must be first instruction" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//... and the operand is neither a hex, a dec, an X'', C'', or a label...
					if (is_hex(parser.get_token(row,2)) == 0 &&
					    parser.get_token(row,2).find_first_not_of("0123456789") != string::npos &&
					    ((to_lower(parser.get_token(row,2))[0] != 'x' && to_lower(parser.get_token(row,2))[0] != 'c') &&
			   		    (parser.get_token(row,2)[1] != '\'' || parser.get_token(row,2)[parser.get_token(row,2).size()-1] != '\'')) &&
					    is_label(parser.get_token(row,2)) == 0) {
						cout << "Error [Line " << index << "]: "
						     << "Invalid operand for BYTE directive" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//BYTE has almost everything it needs
					else {
						//if operand is C'' or X''
						if (parser.get_token(row,2)[1] == '\'' && parser.get_token(row,2)[parser.get_token(row,2).size()-1] == '\'') {
							
							//if it's hex
							if (to_lower(parser.get_token(row,2))[0] == 'x') {
								string tmpstring = parser.get_token(row,2).erase(0,2);
								tmpstring.erase(tmpstring.size()-1,tmpstring.size());
								
								//if contains odd number of hex numbers or is not hex
								if (tmpstring.find_first_not_of("0123456789ABCDEFabcdef") != string::npos ||
							    	    tmpstring.size() % 2 != 0) {
							    		cout << "Error [Line " << index << "]: "
									     << "Invalid hex for BYTE directive" << endl;
									remove(lis.c_str());
									exit(1);
								}
								//BYTE has everything it needs
								else {
									//if label column is not empty
									if (!(parser.get_token(row,0).empty())) {
										//we add it to symbol table
										symbols.add_sym(loccounter, parser.get_token(row,0), "", "");
										nolabel.push_back(loccounter);
									}
										
									//if there is no label
									else 
										nolabel.push_back(loccounter);
										
									//write it all to lis and update loccounter
									write_in_lis(file, parser, index, loccounter, row);
									loccounter += tmpstring.size() / 2;
								}
							}
							//if it's a character constant
							else {
								string tmpstring = parser.get_token(row,2).erase(0,2);
								tmpstring.erase(tmpstring.size()-1,tmpstring.size());
								
								//BYTE has everything it needs
								//if label column is not empty
								if (!(parser.get_token(row,0).empty())) {
									//we add it to symbol table
									symbols.add_sym(loccounter, parser.get_token(row,0), "", "");
									nolabel.push_back(loccounter);
								}
									
								//if there is no label
								else
									nolabel.push_back(loccounter);
									
								//write it all to lis and update loccounter
								write_in_lis(file, parser, index, loccounter, row);
								loccounter += tmpstring.size();
									
							}
						}
						//BYTE has everything it needs
						else {
							//if label column is not empty
							if (!(parser.get_token(row,0).empty())) {
								//we add it to symbol table
								symbols.add_sym(loccounter, parser.get_token(row,0), "", "");
								nolabel.push_back(loccounter);
							}	
							//if there is no label
							else
								nolabel.push_back(loccounter);
							
							//write it all to lis and update loccounter
							write_in_lis(file, parser, index, loccounter, row);
							
							loccounter += 1;
						}
					}
				}
				//if directive is WORD...
				else if(to_lower(parser.get_token(row, 1)) == "word") {
					//throw error if there is no START diretive
					if (start_set == 0) {
						cout << "Error [Line " << index << "]: START directive must be first instruction" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//... and the operand is neither a hex, a dec, an X'', C'', or a label...
					if (is_hex(parser.get_token(row,2)) == 0 &&
					    parser.get_token(row,2).find_first_not_of("0123456789") != string::npos &&
					    ((to_lower(parser.get_token(row,2))[0] != 'x' && to_lower(parser.get_token(row,2))[0] != 'c') &&
			   		    (parser.get_token(row,2)[1] != '\'' || parser.get_token(row,2)[parser.get_token(row,2).size()-1] != '\'')) &&
					    is_label(parser.get_token(row,2)) == 0) {
						cout << "Error [Line " << index << "]: "
						     << "Invalid operand for WORD directive" << endl;
						remove(lis.c_str());
						exit(1);
					}
					//WORD has almost everything it needs
					else {
						//if operand is C'' or X''
						if (parser.get_token(row,2)[1] == '\'' && parser.get_token(row,2)[parser.get_token(row,2).size()-1] == '\'') {
							
							//if it's hex
							if (to_lower(parser.get_token(row,2))[0] == 'x') {
								string tmpstring = parser.get_token(row,2).erase(0,2);
								tmpstring.erase(tmpstring.size()-1,tmpstring.size());
								
								//if contains odd number of hex numbers or is not hex
								if (tmpstring.find_first_not_of("0123456789ABCDEF") != string::npos ||
							    	    tmpstring.size() % 2 != 0) {
							    		cout << "Error [Line " << index << "]: "
									     << "Invalid hex for BYTE directive" << endl;
									remove(lis.c_str());
									exit(1);
								}
								//word has everything it needs
								else {
									//if label column is not empty
									if (!(parser.get_token(row,0).empty())){ 
										//we add it to symbol table
										symbols.add_sym(loccounter, parser.get_token(row,0), "", "");
										nolabel.push_back(loccounter);
									}	
									//if there is no label
									else 
										nolabel.push_back(loccounter);
										
									//write it all to lis and update loccounter
									write_in_lis(file, parser, index, loccounter, row);
									loccounter += (tmpstring.size() / 2)*3;
								}
							}
							//if it's a character constant
							else {
								string tmpstring = parser.get_token(row,2).erase(0,2);
								tmpstring.erase(tmpstring.size()-1,tmpstring.size());
								
								//WORD has everything it needs
								//if label column is not empty
								if (!(parser.get_token(row,0).empty())) {
									//we add it to symbol table
									symbols.add_sym(loccounter, parser.get_token(row,0), "", "");
									nolabel.push_back(loccounter);
								}	
								//if there is no label
								else
									nolabel.push_back(loccounter);
									
								//write it all to lis and update loccounter
								write_in_lis(file, parser, index, loccounter, row);
								loccounter += tmpstring.size()*3;
									
							}
						}
						//WORD has everything it needs
						else {
							//if label column is not empty
							if (!(parser.get_token(row,0).empty())) {
								//we add it to symbol table
								symbols.add_sym(loccounter, parser.get_token(row,0), "", "");
								nolabel.push_back(loccounter);
		
							}
							//if there is no label
							else
								nolabel.push_back(loccounter);
							
							//write it all to lis and update loccounter
							write_in_lis(file, parser, index, loccounter, row);
							loccounter += 3;
						}
					}
				}
				
				else {
					cout << "Error [Line " << index << "]: "
					     << e.getMessage() << endl;
					remove(lis.c_str());
					exit(1);
				}
			}
		}
		
		index++;
	}
	//throw error if there is no END directive
	if (end_set = 0) {
		cout << "Error [Line " << index << "]: Program must have an END directive"  << endl;
		remove(lis.c_str());
		exit(1);
	}
}

string sicxe_asm::to_lower(string direct) {
	string tmp = direct;
	transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
	return tmp;
}

bool sicxe_asm::is_label(string label) {
	if (isalpha(label[0]) == 0)
		return 0;
	for (unsigned int i = 0; i < label.size(); i++) {
		if (isalnum(label[i]) == 0)
			return 0;
	}
	return 1;
}

bool sicxe_asm::is_hex(string operand) {
	if (operand[0] != '$')
		return 0;
	operand.erase(0,1);
	if (operand.find_first_not_of("0123456789ABCDEF") != string::npos)
		return 0;
	return 1;
}

int *sicxe_asm::get_reg_num(string registers, unsigned int index, int num_reg) {
	//int reg_machine [2] = {};
	int size =1;
	string first_reg;
	string second_reg;
	registers = to_lower(registers);
	if(num_reg != 3){
		if (registers[0] == 'a') 
			first_reg = "0";
		else if (registers[0] == 'x')
			first_reg = "1";
		else if (registers[0] == 'l')
			first_reg = "2";
		else if (registers[0] == 'b')
			first_reg = "3";
		else if (registers[0] == 's') {
			if (registers[1] == ',')
				first_reg = "4";
			else if (registers[1] == 'w'){
				first_reg = "9";
				size =2;
			}
			else {
				cout << "[Line " << index << "]Error: not a valid register" << endl;
				remove(lisfilename.c_str());
				exit(1);
			}
		}	
		else if (registers[0] == 't')
			first_reg = "5";
		else if (registers[0] == 'p') {
			if (registers[1] == 'c'){
				first_reg = "8";
				size =2;
			}
			else {
				cout << "[Line " << index << "]Error: not a valid register" << endl;
				remove(lisfilename.c_str());
				exit(1);
			}
		}
		else {
			cout << "[Line " << index << "]Error: not a valid register" << endl;
			remove(lisfilename.c_str());
			exit(1);
		}
	}
	//for SVC
	if(num_reg == 3){
		stringstream ss;
		int tmp;
		if(registers[0] == '$'){
			registers.erase(0,1);
			if(registers.size() > 1){
				cout << "[Line " << index << "]Error: operand number is invalid number 0-15" << endl;
				remove(lisfilename.c_str());
				exit(1);
			}
			if(registers.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos){
				cout << "[Line " << index << "]Error: operand number is invalid number 0-15" << endl;
				remove(lisfilename.c_str());
				exit(1);
			}
			
			ss.clear();
			ss << std::hex << registers;
			ss >> tmp;
			reg_machine[0] = tmp;
	
			return reg_machine;
		}
		else if((registers.find_first_of("0123456789") != std::string::npos) && 
		(registers.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == std::string::npos) &&
		(registers.find_first_of("abcdefghijklmnopqrstuvwxyz") == std::string::npos)) {
			if(registers.size() > 1){
				if(registers.size() > 2 || (registers[0] != '1' && registers[0] != '0') ) {
					cout << "[Line " << index << "]Error: operand number is invalid number 0-15" << endl;
					remove(lisfilename.c_str());
					exit(1);	
				}
				if(registers[0] == '1' && (registers[1] == '6' || registers[1] == '7' || 
				registers[1] == '8' || registers[1] == '9')){
					cout << "[Line " << index << "]Error: operand number is invalid number 0-15" << endl;
					remove(lisfilename.c_str());
					exit(1);
				}
			}
			first_reg = registers;
		}
		else {
			cout << "[Line " << index << "]Error: not a valid number for SVC 0-15" << endl;
			remove(lisfilename.c_str());
			exit(1);
		}
		
	}
	//for SHIFTL and SHIFTR
	if(num_reg == 4) {
			second_reg = registers.erase(0,size+1);
			//NEEDS to check whole number than minus by 1 for SHIFTL and SHIFTR
			if((registers.find_first_of("0123456789") != std::string::npos) && 
			(registers.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == std::string::npos) &&
			(registers.find_first_of("abcdefghijklmnopqrstuvwxyz") == std::string::npos)){
				if(registers.size() > 1){
					if(registers.size() > 2 || (registers[0] != '1' && registers[0] != '0') ){
						cout << "[Line " << index << "]Error: operand number is invalid number 1-16" << endl;
						remove(lisfilename.c_str());
						exit(1);	
					}
					if(registers[0] == '1' && (registers[1] == '7' || registers[1] == '8' || registers[1] == '9')){
						cout << "[Line " << index << "]Error: operand number is invalid number 1-16" << endl;
						remove(lisfilename.c_str());
						exit(1);
					}
					if(registers[0] == '0' && registers[1] == '0'){
						cout << "[Line " << index << "]Error: operand number is invalid number 1-16" << endl;
						remove(lisfilename.c_str());
						exit(1);
					}
				}
				if(registers.size() == 1 && registers[0] == '0'){
					cout << "[Line " << index << "]Error: operand number is invalid number 1-16" << endl;
					remove(lisfilename.c_str());
					exit(1);
				}
				
				int tmp = atoi(second_reg.c_str());
				tmp = tmp - 1;
				ostringstream ss;
     				ss << tmp;
				second_reg = ss.str();
							
			}
			else {
				cout << "[Line " << index << "]Error: not a valid register for SHIFTL/R" << endl;
				remove(lisfilename.c_str());
				exit(1);
			}
			reg_machine[1] = atoi(second_reg.c_str());
	}
	if(num_reg == 2) {
		if (registers[registers.size()-1] == 'a')
			second_reg = "0";
		else if (registers[registers.size()-1] == 'x')
			second_reg = "1";
		else if (registers[registers.size()-1] == 'l')
			second_reg = "2";
		else if (registers[registers.size()-1] == 'b')
			second_reg = "3";
		else if (registers[registers.size()-1] == 's')
			second_reg = "4";
		else if (registers[registers.size()-1] == 't')
			second_reg = "5";
		else if (registers[registers.size()-1] == 'c') {
			if (registers[registers.size()-2] == 'p')
				second_reg = "8";
			else {
				cout << "[Line " << index << "]Error: not a valid register" << endl;
				remove(lisfilename.c_str());
				exit(1);
			}
		}
		else if (registers[registers.size()-1] == 'w') {
			if (registers[registers.size()-2] == 's')
				second_reg = "9";
			else {
				cout << "[Line " << index << "]Error: not a valid register" << endl;
				remove(lisfilename.c_str());
				exit(1);
			}
		}
		else {
			cout << "[Line " << index << "]Error: not a valid register" << endl;
			remove(lisfilename.c_str());
			exit(1);
		}
		
		reg_machine[1] = atoi(second_reg.c_str());
	}
	
	reg_machine[0] = atoi(first_reg.c_str());
	
	
	return reg_machine;
}

string sicxe_asm::formatHexNumber(int n, int digits){
	stringstream s;
	s << hex << setw(digits) << setfill('0') << n;
	string toReturn = s.str();
	transform(toReturn.begin(), toReturn.end(), toReturn.begin(), ::toupper);
	return toReturn;
}
/******************************PRIVATE METHODS END********************************/


int main(int argc, char *argv[]) {
    	sicxe_asm assemble;
	
	if(argc != 2) {
        	cout << "Error, you must supply the name of the file " <<
        		"to process at the command line." << endl;
        	exit(1);
	}
    	
	assemble.first_pass(argc, argv[1]);
	assemble.second_pass(argc, argv[1]);
	
	cout << argv[1] << " has been successfully assembled!" << endl;
    
	return 0;
}
