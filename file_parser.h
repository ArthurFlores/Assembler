/*  file_parser.h
	
    Arthur Flores, Christian Barroso , Jerald Tomada, Nick Schneider
    masc0855
    Team New Mexico
    prog1
    
    CS530 Spring 2016
    Alan Riggins 
*/

#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include "file_parse_exception.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <iomanip>
#include <sstream>

#define MAX_LABEL_SIZE 8

using namespace std;

class file_parser {
    public:
        // ctor, filename is the parameter.  A driver program will read
        // the filename from the command line, and pass the filename to
        // the file_parser constructor.  Filenames must not be hard-coded.
        file_parser(string); 
        
        // dtor
        ~file_parser();
        
        // reads the source file, storing the information is some
        // auxiliary data structure you define in the private section. 
        // Throws a file_parse_exception if an error occurs.
        // if the source code file fails to conform to the above
        // specification, this is an error condition.     
        void read_file();    
        
        // returns the token found at (row, column).  Rows and columns
        // are zero based.  Returns the empty string "" if there is no 
        // token at that location. column refers to the four fields
        // identified above.
        string get_token(unsigned int, unsigned int);
        
        // prints the source code file to stdout.  Should not repeat 
        // the exact formatting of the original, but uses tabs to align
        // similar tokens in a column. The fields should match the 
        // order of token fields given above (label/opcode/operands/comments)
        void print_file();
        
        // returns the number of lines in the source code file
        int size();
        
    private:
        // your variables and private methods go here
	string get_comment(int,int,string,string);
	void check_label(int,string,int,string);
	string error_string(string,int);
	
	struct s_line {		//This struct places the word from line in correct spot
		string label;
		string opcode;
		string operand;
		string comment;
		
		s_line(){
			label = "";
			opcode = "";
			operand = "";
			comment = "";
		}
		
		void read_word(string &word,int type){
			switch(type){
				case 0:
					label = word;
					break;
				case 1:
					opcode = word;
					break;
				case 2: 
					operand = word;
					break;
				case 3:
					comment = word;
					break;
			}
			word.clear();
		}
	};
	vector<string> contents; //Vector for contents of the input file
	vector<s_line> tokens; //Vector of the struct lines
};

#endif 
