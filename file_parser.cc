/*  file_parser.cc
    
    Arthur Flores, Christian Barroso , Jerald Tomada, Nick Schneider
    masc0855
    Team New Mexico
    prog1
    
    CS530 Spring 2016
    Alan Riggins
*/ 


#include "file_parser.h"

using namespace std;

       // ctor, filename is the parameter.  A driver program will read
        // the filename from the command line, and pass the filename to
        // the file_parser constructor.  Filenames must not be hard-coded.
file_parser::file_parser(string filename){
	ifstream infile;
	string line;
	
	//if(argc != 2) print_error("There are too many arguments!"); //Captures error for arguments
	infile.open(filename.c_str(),ios::in); //reads in file
	if(!infile) throw file_parse_exception("File failed to open: " + filename); //There was no file, possibly place error for no file exists
	while(!infile.eof()){			//This while loop puts each line within the vector contents
		getline(infile,line);
		contents.push_back(line);
	}
	infile.close();
}
        
        // dtor
file_parser::~file_parser(){}
        
        // reads the source file, storing the information is some
        // auxiliary data structure you define in the private section. 
        // Throws a file_parse_exception if an error occurs.
        // if the source code file fails to conform to the above
        // specification, this is an error condition.     
void file_parser::read_file(){
	string delimiters = " \t";
	for( unsigned int i=0; i < contents.size(); i++){
		s_line read_line;
		unsigned int type=0;
		string line = contents[i];
		if(isspace(line[0])) //This checks if first char on line is whitespace
			type++;
		int last = line.find_first_not_of(delimiters,0);
		int first = line.find_first_of(delimiters,last);
		string token = "";
		while(first != -1 || last != -1){
			token = line.substr(last,first-last);
			if(token.find(".") != std::string::npos){ 
				// if you find a . for comment you take rest of line as comment
				if(token.at(0) == '.'){ //If first char in token is a . then make comment
					type = 3;
					token = get_comment(first,last,token,line);
					read_line.read_word(token,type);
					break;
				}
				//if there was something in front of . when discovered
				unsigned int k;
				for(k = 0; k < token.size(); k++)
					if(token.at(k)=='.')
						break;
				line.insert(last+k," ");
				first = line.find_first_of(delimiters,last);
				continue;
			}
			if(type >= 3)	//if there are too many arguments
				throw file_parse_exception(error_string("Too many tokens at line: ",i+1));
			if(token.find("'") != std::string::npos){  //Looks for a quote
				if(token.find("'") == token.rfind("'")){	//This finds the closing quote 
					string tmp = token;
					first = line.find_last_of("'");
	;				int findq = line.find_first_of("'"); //goes to begining of line to find position of first quote
					if(findq == first)
						throw file_parse_exception(error_string("There is no closing quote on line: ",i+1));
					first++;
					if((int)line.size() != first && line.at(first) == '.')
						throw file_parse_exception(error_string("Cant have a commnet start after a quote on line: ",i+1));
					else if((int)line.size() != first && line.at(first) != ' ')
						first = line.find_first_of(delimiters,first);
					if(first != -1 || last != -1)
						token = line.substr(last,first-last);
					last = first;
				}
			}	
			check_label(type,token,i,line);
			read_line.read_word(token,type++); // places token in correct position in struct
			last = line.find_first_not_of(delimiters,first);
			first = line.find_first_of(delimiters,last);
		}
		tokens.push_back(read_line);
	}
}    
        
        // returns the token found at (row, column).  Rows and columns
        // are zero based.  Returns the empty string "" if there is no 
        // token at that location. column refers to the four fields
        // identified above.
string file_parser::get_token(unsigned int row, unsigned int column){
	if(row >= tokens.size()) throw file_parse_exception(error_string("No tokens exist on row: ",row));
	switch(column){
		case 0:
			return tokens[row].label;
		case 1:
			return tokens[row].opcode;
		case 2:
			return tokens[row].operand;
		case 3:
			return tokens[row].comment;
		default:
			throw file_parse_exception(error_string("There are only 4 columns(hint: this is zero based), no token found at row: ", row)+error_string(" column: ",column));
			return "";
	}
	
}
       
        // prints the source code file to stdout.  Should not repeat 
        // the exact formatting of the original, but uses tabs to align
        // similar tokens in a column. The fields should match the 
        // order of token fields given above (label/opcode/operands/comments)
void file_parser::print_file(){
	for(unsigned int i=0;i < tokens.size(); i++){
		cout.setf(ios::left);
		cout << setw(20) << tokens[i].label  << setw(14) << tokens[i].opcode 
		<< setw(14) << tokens[i].operand << setw(20) << tokens[i].comment << endl;
	}
}
        
        // returns the number of lines in the source code file
int file_parser::size(){
	return tokens.size();
}
        
////////////////////PRIVATE VARIABLES////////////////////////////////////

//This method makes the comment when a "." is found
string file_parser::get_comment(int first, int last, string token, string line){
	string tmp = token;
	string delimiters = "";
	int l = line.find_first_not_of(delimiters,first);
	int f = line.find_first_of(delimiters,last);
	if( f != -1 || l != -1){
		tmp = line.substr(l,f-l);
		tmp = token + tmp;	
	}
	return tmp;
}

//This method checks if the label is valid or not
void file_parser::check_label(int type, string token, int i, string line){
	if(type == 0 && !isalpha(token.at(0))) //This will make sure first char of label is a letter
		throw file_parse_exception(error_string("Invalid label: Label must begin with a letter on line: ",i+1));
	if(type == 0)
		for(unsigned int j=1; j < token.size(); j++) //This  will prevent a nonalnum char to be passed with a label
			if(!isalnum(line[j])) 
				throw file_parse_exception(error_string("Invalid label: Only letter and Digits allowed in label on line: ",i+1));
	
}

//This method will make the error messages for exceptions. This will convert an int to a string and return string
string file_parser::error_string(string message, int num){
	stringstream str;
	string tmp;
	str << num;
	str >> tmp;
	message += tmp;
	return message;
}



