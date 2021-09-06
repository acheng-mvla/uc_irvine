#include <bits/stdc++.h>
using namespace std;

ifstream fin; 
ofstream fout;
const string WHITESPACE = " \n\r\t\f\v";
string trimSpaces(string line){
    int start = line.find_first_not_of(WHITESPACE);
    if(start != string::npos) line = line.substr(start);
    int last = line.find_last_not_of(WHITESPACE);
    if(last != string::npos) line = line.substr(0,last+1);
    return line;
}
int main(){
	fin.open("page1.txt");
	fout.open("titles.txt");
	if(fin.is_open()){
		string line;
		while(getline(fin,line)){
			if(tolower(line).find("author index")){
				break;
			}
			int i = line.find(" . .");
			if(i != string::npos){
				continue;
			}
			string t = trimSpaces(line.substr(0,i+1));
			fout << t << "\n";
		}
	}
}