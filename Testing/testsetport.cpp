#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h> 

using namespace std;

ifstream testFile;
ofstream results;
const string RESULTS_FILE = "results.txt";
const string COMMANDS_FILE = "testCommands.txt";

//write each command, results, return to a file
void testCommand(string testCmd) {
	results.open(RESULTS_FILE.c_str(), fstream::app);		//open file each time, otherwise << overwrites output written by system
	results << testCmd << endl;								//write argument string to file
	testCmd += " >>"+ RESULTS_FILE;							//add >>results.txt to write results to text file
	int statusCode = system(testCmd.c_str()) / 256;			//set status code to argument status code
	results << "return " << statusCode << endl << endl;		//display return value to results.txt
	results.close();										//close file each time, otherwise << overwrites output written by system
}

//check if files are the same
int testFiles() {
	string sysCom = "diff " + RESULTS_FILE + " " + COMMANDS_FILE;
	int statusCode = system(sysCom.c_str()) / 256;
	if (statusCode != 0) {
		cout << "testing failed" << endl;
		return 1;
	}
	cout << "testing success" << endl;
}

//read each line from file and pass to testCommand function
int evaluateFile() {
	string testCmd;
	results.open(RESULTS_FILE.c_str(), fstream::trunc);				//delete previous results
	results.close();
	testFile.open(COMMANDS_FILE.c_str());
	
	while(getline(testFile, testCmd)) {								//read all commands from text file and run each one until end of file
		testCommand(testCmd);
	}

	testFile.close();

	return testFiles();
}


//test setport program
int main(int argc, char* args[]) {
	return evaluateFile();
}




