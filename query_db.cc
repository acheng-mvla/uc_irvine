
/**
 * @file
 * @brief A file to create a database for 40th annual cognitive science society meeting
 *        paper summaries.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <bits/stdc++.h>
using namespace std;

/**
 * @brief data structure for one paper summary
 */
const string WHITESPACE = " \n\r\t\f\v";
string trimSpaces(string line){
    int start = line.find_first_not_of(WHITESPACE);
    if(start != string::npos) line = line.substr(start);
    int last = line.find_last_not_of(WHITESPACE);
    if(last != string::npos) line = line.substr(0,last+1);
    return line;
}
map<string,int> titles;

class paper_summary {
public:
    string title;
    vector<string> authors;
    string keywords;
    vector<string> references;
    string abstract;
};

/**
 * @brief Find whether a string is a page, which should look like "1234"
 */
int last_page_number{0};
enum RefState {Found, Wait, PageNumber};
RefState waitForReferences(string line){
    if(trimSpaces(line).compare("References") == 0){
        return RefState::Found;
    } 
    if(findPageNumber(line)){
        return RefState::PageNumber;
    }
    return RefState::Wait;
}
bool findPageNumber(const string line) {
    stringstream ss(line);
    int x;
    ss>>x;
    if(ss.fail()) {
        return false;
    } else {
        if(line.length() > 5 || (line.find('%') != string::npos) 
        || line.find('.') !=string::npos) {
            return false;
        }

        if(last_page_number != -1 && x < last_page_number && x != last_page_number+1) {
            // cout << "Wrong page number: " << line << endl;
            return false;
        }

        cout << "got page number " << x << ", line:" << line << endl;
        last_page_number = x;
        return true;
    }
}

/// State machine states definition
enum State {Initial, PageNumber, Title, Authors, Keywords, Abstract, References, Wait, None};
/// One tempory paper summary
paper_summary temp_paper_summary{};


/**
 * @brief Process one line of ASCII text
 */
static State lastState{State::Initial};
static State state{State::Initial};
static int onPaper = 1;
void processOneLine(string line, map<string, paper_summary>& db) {
    // Initial state

    bool done = false;
    while(true){
        switch (state) {
            case State::Initial:
                // Search for PageNumber
                if (findPageNumber(line)) {
                    lastState = state;
                    state = State::PageNumber;
                }
                return;

            case State::PageNumber: 
                // Initialize one temp record
                if(titles.at(trimSpaces(line)) == onPaper+1){
                    lastState = state;
                    state = State::Title;
                    db.insert(pair<string,paper_summary>(temp_paper_summary.title,temp_paper_summary));
                    temp_paper_summary.title = "";
                    temp_paper_summary.authors.clear();
                    temp_paper_summary.keywords = "";
                }
                else{
                    state = lastState;
                    lastState = State::PageNumber;
                }
                break;
                // Falling through to State::Title, always assume next line after page number is title. We'll
                // discard it if it's not the case. We are expecting to find title and keywords
                // on the same page.

            case State::Title:
                temp_paper_summary.title = trimSpaces(line);
                state = State::Authors;
                return;
            case State::Abstract:
                int keyPos = line.find("Keywords:");
                if(keyPos != string::npos){
                    if(keyPos != 0){
                        temp_paper_summary.abstract += line.substr(0,keyPos);
                    }
                    line = line.substr(keyPos);
                    lastState = state;
                    state = State::Keywords;
                    break;
                }
                else if(findPageNumber(line)) {
                    lastState = state;
                    state = State::PageNumber;
                    return;
                }
                else if(waitForReferences(line) == RefState::Found) {
                    lastState = state;
                    state = State::References;
                    return;
                }
                else {
                    temp_paper_summary.abstract += trimSpaces(line) + " ";
                    return;
                }
            case State::Authors:
                if(tolower(trimSpaces(line)).compare("abstract") == 0) {
                    lastState = state;
                    state = State::Abstract;
                    return;
                }
                else if(tolower(line).find("keywords:") != string::npos) {
                    lastState = state;
                    state = State::Keywords;
                    break;
                    // Need go to Keywords state. Fall through to State::Keywords.
                } else if(findPageNumber(line)) {
                    // Discard current temp record.
                    lastState = state;
                    state = State::PageNumber;
                    return;
                } else {
                    temp_paper_summary.authors.push_back(line);
                    return;
                }
            case State::Keywords:
                temp_paper_summary.keywords+=line;
                cout << "Keywords State: " << line << endl;
                // Add this record to DB
                lastState = state;
                state = State::Wait;
                break;
            case State::References:
                if(findPageNumber(line)){
                    lastState = state;
                    state = State::PageNumber;
                    return;
                }
                temp_paper_summary.references.push_back(line);
            case State::Wait:
                RefState w = waitForReferences(line);
                if(w == RefState::PageNumber){
                    lastState = state;
                    state = State::PageNumber;
                }
                else if(w == RefState::Found){
                    lastState = state;
                    state = State::References;
                }
                return;
            default:
                cout << "Error state: shall never be here!" << endl;
                break;
        }
    }
}

int main() {
    ifstream title("files/titles.txt");
    if(title.is_open){
        string line;
        int i = 1;
        while(getline(title,line)){
            titles.insert(pair<string,int>(line,i));
            i++;
        }
    }
    map<string, paper_summary> db;

    ifstream file("files/c4.txt");

    int line_cnt{0};
    if(file.is_open()) {
        string line;
        while(getline(file, line)) {
            processOneLine(line, db);
            ++line_cnt;
        }
        file.close();
    }

    cout << ">>> number of lines " << line_cnt << " <<<" << endl;
    return 0;
}

