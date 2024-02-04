#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>

using namespace std;

vector<string>coutStream;

void clearString(string &str) {
	for (int i = 0; i < str.size(); i++) {
		if (!(str[i] >= 32 && str[i] <= 126)) {
			str.erase(i--);
		}
	}
}
void loadCoutStream(string input) {
	clearString(input);
	coutStream.push_back(input);
}
void printCoutStream() {
	for (int i = 0; i < coutStream.size(); i++) {
		fflush(stdin);
		fflush(stdout);
		printf("%s \n", coutStream[i].c_str());
	}
		
	coutStream.clear();
}
void readFile(string file, vector<string> &fileData) {
	std::ifstream ifs(file, std::ios::in);
	if (!ifs.is_open()) {
		loadCoutStream("Failed to open file :" + file);
		return;
	}

	string saveStr = "";
	while (std::getline(ifs, saveStr)) {
		fileData.push_back(saveStr);
	}
	ifs.close();
}
void fixFile(string target,string name, vector<string> &fileData) {
	for (int i = 0; i < fileData.size(); i++) 
		if (fileData[i].find(target) != string::npos) 
			fileData[i] = target + "   " + name;
}
string addMark(string name) {
	return "\"" + name + "\"";
}
void assignFile(string file, vector<string> &fileData) {
	ofstream myfile;
	myfile.open(file);
	for (int i = 0; i < fileData.size(); i++)
		myfile << fileData[i] << endl;
	myfile.close();
}
void fixDRC(string cell, string DRCrul, string GDS) {
	vector<string> fileData;
	readFile(DRCrul, fileData);
	string markGDS = addMark(GDS);
	fixFile("LAYOUT PATH", markGDS, fileData);
	fixFile("LAYOUT PRIMARY", cell, fileData);
	assignFile(DRCrul, fileData);
}
void runDRC(string DRCrul) {
	system("rm drc_report");
	string instruction = "calibre ¡Vdrc " + DRCrul;
	system(instruction.c_str());
}
void printErrorMessage(string cellName) {
	vector<string> fileData;
	readFile("drc_report", fileData);
	for (int i = 1; i < fileData.size(); i++) {
		if (fileData[i].find("TOTAL Result Count") != string::npos) {
			int pos = fileData[i].find("TOTAL Result Count =");
			for (int j = pos; j < fileData[i].size(); j++) {
				if ('0' <= fileData[i][j] && fileData[i][j] <= '9') {
					if (fileData[i][j] != '0') {
						if (fileData[i].find("ACTIVE.LUP.1") == string::npos) {
							loadCoutStream(cellName + " error :");
							loadCoutStream(fileData[i]);
						}
					}
						
				}
			}
		}
	}
}

void autoDRC(string GDS, string DRCrul,vector<string> cellList) {
	for (int i = 0; i < cellList.size(); i++) {
		
		string cell = cellList[i];
		clearString(cell);
		fixDRC(cell, DRCrul, GDS);
		runDRC(DRCrul);
		printErrorMessage(cell);
		string a = "finish check " + cell;
		loadCoutStream(a);
	}
}
void readCellList(vector<string> &cellList) {
	vector<string> fileData;
	readFile("cellList", fileData);
	for (int i = 0; i < fileData.size(); i++) 
		cellList.push_back(fileData[i]);
}
int main(int argc, char *argv[]) {
	if (argc < 2) {
		cout << "please add ARGV of GDS,DRC.rul " << endl;
		return -1;
	}
	string GDS = argv[1];
	string DRCrul = argv[2];
	/*string GDS = "AO_fin21.gds";
	string DRCrul = "finFlex_DRC.rul";*/
	vector<string> cellList;
	readCellList(cellList);
	if (cellList.size() == 0) {
		cout << "please add cellList file" << endl;
		return -1;
	}
	autoDRC(GDS,DRCrul, cellList);
	printCoutStream();
}