#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cctype>
#include <regex>
#include <string>
#include <vector>
#include <unistd.h>
using namespace std;

vector<string> msg;
string const USAGE = "usage";					//usage file name
string const ABOUT = "about";					//about file name
string const DEF_ENV = "PORT";					//default environment variable
string sysLocale;
string langDir;									//language directory
regex re("^([a-z]{2})(_[A-Z]{2})?(\\..+)?");
regex fileName("/([^/]*)$");
string const LOC_ENV[] = {						//check environment variables for locale
	"LANGUAGE",
	"LC_ALL",
	"LC_MESSAGE",
	"LANG"
};
enum {
   LISTENING,									//Listening on port: 
   INVALID_PORT,								//Invalid port
   INCORRECT_FLAG,								//Non-valid Flag
   ARG_START,									//First argument must start with a '-'
   TOO_MANY_ARG,								//Too many arguments given
   NO_ENV,										//environment variable does not exist
   VERSION,										//version
   COUNT										//count number of variables
};


void getDirectory(char* args){
	langDir = args;
	langDir = regex_replace(langDir, fileName, "/");			//replace end of file location to /
	langDir += "Language-Files/";								//set language directory
}

string getLocale(){
	for (int i = 0; i < 4; ++i){
		if(getenv(LOC_ENV[i].c_str()) != NULL && regex_match(getenv(LOC_ENV[i].c_str()), re)){
			return getenv(LOC_ENV[i].substr(0, 2).c_str());
		}
  	}
	return "en";												//default to english
}

void version(){
	cout << msg[VERSION] << "3.2" << endl;
}


//displays text from file to console
void fromFile(string fileType){
	ifstream in;
	string inLine;
	
	string fileLoc = langDir + "setport_"+fileType+"_"+ sysLocale +".txt";
	in.open(fileLoc.c_str());
	if (!(in.is_open())){
		cout << fileType << " not found for locale, defaulting to english" << endl;
		fileLoc = langDir + "setport_"+fileType+"_en.txt";		//default to english
		in.open(fileLoc.c_str());
		if (!(in.is_open())){									//error out of english file is missing
			cout << "English file not found or incorrect for "+fileType << endl;
			return;
		}
	}
	while(getline(in, inLine)) {								//read all commands from text file and run each one until end of file
		cout << inLine << endl;
	}
}

void getMessages(){
	ifstream in(langDir + "setport_msg_"+ sysLocale +".txt");
	if (!(in.is_open())){return;}
	string line;
	while (!in.eof()) {
		getline(in, line);										//take line from file and send to vector
		msg.push_back(line);
	}
}

//converts char value into it's integer value
int charToInt(char arg[]){
    int total = 0;
    int length = strlen(arg);
    for (int i = 0; i < length; ++i){
        if (isdigit(arg[i]) && i < 6){							//can't convert non-digit to int, don't want digits higher than 6
																//for each number in the array, multiply the total by 10 to represent it's location 
																//subtract 48 from ascii value to cast
            total = total*10 + (int)arg[i]-48;
        } 
        else{return -1;}										//-1 is an error value, can not have port less than 0
    }
    return total;
}

//"changes" port given by user
int changePort(int port){
    cout << msg[LISTENING] << port << endl;
    return 0;
}

//evaluates port value given by user
int evaluatePort(char arg[]){									//value must be between 1 and 65355
    if (charToInt(arg) > 0 && charToInt(arg) <= 65535){return changePort(charToInt(arg));} 
    cout << msg[INVALID_PORT] << endl;
    fromFile(USAGE);
    return 1;
}

//compares two char arrays and returns bool value
bool compareArray(char arg[], string flag){ 					//check if argument and flag match
	const char *cflag = flag.c_str();
	if (strcmp(arg, cflag) == 0) return true;
    return false;											
}

//evaluates argument given by user
int evaluateArgument(int argc, char* args[]){
	getDirectory(args[0]);										//get file directory
	sysLocale = getLocale();
	getMessages();
	if (msg.size() != COUNT){ 									//default to english if file is bad
		cout << "Message file was missing or incorrect, defaulting to english" << endl;
		sysLocale = "en";
		msg.clear();
		getMessages();
		if (msg.size() != COUNT){								//exit program if english file is not found
			cout << "English files not found or incorrect" << endl;
			return 1;
		}
	}
	char* flag = args[1];
	if (argc == 1) {											//display usage screen if there are no parameters
		fromFile(USAGE);
		return 0;
	}
	//can probably remove, kept for more clarity on error
	if (flag[0] != '-') {										//check first character of second argument
		cout << msg[ARG_START] << endl;
		fromFile(USAGE);
		return 1;
	}
	if (compareArray(flag, "-h") || compareArray(flag, "--help") || compareArray(flag, "-?")) {
		if (argc == 2){
			fromFile(USAGE);
			return 0;
		}
		cout << msg[TOO_MANY_ARG] << endl;
		fromFile(USAGE);
		return 1;
	}
	if (compareArray(flag, "-v") || compareArray(flag, "--version")){
		version();
		return 0;
	}
	if (compareArray(flag, "-!") || compareArray(flag, "--about")){
		fromFile(ABOUT);
		return 0;
	}
	if (!compareArray(flag, "-p") && !compareArray(flag, "--port")) {
		cout << msg[INCORRECT_FLAG] << endl;
		fromFile(USAGE);
		return 1;
	}
	if (argc > 2 && compareArray(args[2], "-e") && argc < 5){					//first argument must be port at this point, check for flag 
		if (argc == 4){															//	and args are less than maximum allowable parameters
			if (getenv(args[3]) != NULL){return evaluatePort(getenv(args[3]));}	//send second argument to be evaluated
			cout << msg[NO_ENV] << endl;				
			fromFile(USAGE);
			return 1;
		}
		return evaluatePort(getenv(DEF_ENV.c_str()));
	}
	if (argc == 3){return evaluatePort(args[2]);}
	cout << msg[INVALID_PORT] << endl;
	fromFile(USAGE);
	return 1;
}

//program sets port
int main(int argc, char* args[]){
	//cout << args[0] << endl;
    return evaluateArgument(argc, args);
}
