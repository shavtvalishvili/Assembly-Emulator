#include "AssemblyParser.h"
#include <iostream>
#include <thread>
#include "Constants.hpp"
const char kFunctionBraceOpen = '<';
const char kFunctionBraceClose = '>';

// Public

AssemblyParser::AssemblyParser() {
    error = false;
}

AssemblyParser::~AssemblyParser() {
    return;
}

void AssemblyParser::InitializeParser(string filename) {
    // Input Stream Initialization
    my_file.open(filename, ifstream::binary);
    if (!my_file.good()) {
        cout << "<< ! Error opening file ! >>" << endl;
        my_file.close();
        error = true;
    } else {
        error = false;
    }
}

vector<string> AssemblyParser::GetVector() {
    vector<string> result;

    map<string, int> functions;
    vector<int> function_call_lines;

    // Input Stream Storage Variable
    string line;
    int counter = 0;
    while (getline(my_file, line)) {
        // Uncommenting and Trimming
        // C++ substr returns the same unchanged string if it has 0 and -1 as arguments
        line = line.substr(0, line.find(kCommentChar));
        Trim(line);
        // If line is not an empty string
        if (line != "") {
            // Check
            string error_message;
            if (IllegalCode(line, error_message)) IllegalCodeError(counter, error_message);

            RemoveMultipleSpaces(line);
            CorrectMemoryAccessBraces(line);
            AddNecessarySpaces(line, "+-*/=");
            TranslateNegativeValueSign(line);

            int brace_o_idx = line.find(kFunctionBraceOpen);
            // Function Call or Function Definition
            if (brace_o_idx != -1) {
                // Stores function destination
                if (line[0] == kFunctionBraceOpen) {
                    string func_name = line.substr(1, line.length() - 2);
                    functions[func_name] = counter * 4;
                    continue;
                // Replaces function name with its destination
                } else {
                    function_call_lines.push_back(counter);
                }
            }
            result.push_back(line);
            counter++;
        }
    }

    my_file.close();

    TranslateFunctionCalls(function_call_lines, result, functions);

    return result;
}

bool AssemblyParser::ErrorOccured() {
	return error;
}

// Private

void AssemblyParser::Trim(string &str) {
	int l_cut = str.find_first_not_of(" \n\r\t");
	if (l_cut == -1) {
		str = "";
		return;
	}
	int r_cut = str.find_last_not_of(" \n\r\t") + 1 - l_cut;
	str = str.substr(l_cut, r_cut);
}

void AssemblyParser::RemoveMultipleSpaces(string &line) {
    // Change Tabs
    int tab_idx; tab_idx = line.find('\r');
    while (tab_idx != -1) {
        line.replace(tab_idx, 1, " ");
        tab_idx = line.find('\r');
    }
    // Remove Multiple Spaces
    string tmp = ""; tmp += line[0];
    for (int i = 1; i < line.length(); i++) {
        if (!(line[i] == ' ' && line[i - 1] == ' ')) {
            tmp += line[i];
        }
    }

    line = tmp;
}

void AssemblyParser::CorrectMemoryAccessBraces(string &line) {
    int mem_open_idx = line.find("[ ");
    if (mem_open_idx != -1) line.replace(mem_open_idx, 2, "[");
    int mem_close_idx = line.find(" ]");
    if (mem_close_idx != -1) line.replace(mem_close_idx, 2, "]");
}

void AssemblyParser::AddNecessarySpaces(string &line, string symbols) {
    for (int i = 0; i < symbols.length(); i++) {
        char symbol = symbols[i];
        int idx = line.find(symbol);
        if (idx != -1) {
            if (line[idx - 1] != ' ')  {
                line.insert(idx, " ");
                idx++;
            }
            if (line[idx + 1] != ' ') line.insert(idx + 1, " ");
        }
    }
}

void AssemblyParser::TranslateNegativeValueSign(string &line) {
    int negative_sign_idx = line.find(kNegativeSign);
    while (negative_sign_idx != -1) {
        string f_half = line.substr(0, negative_sign_idx);
        string s_half = line.substr(line.find_first_not_of(' ', line.find("NEG") + 3));
        line = f_half + '-' + s_half;
        negative_sign_idx = line.find(kNegativeSign);
    }
}

bool AssemblyParser::IllegalCode(string line, string &error_message) {
    //Case M
    int m_idx = line.find(kMemoryAccess);
    if (m_idx != -1) {
        // If two M-s (LOAD and STORE)
        if (line.find(kMemoryAccess, m_idx + 1) != -1) {
            error_message = "Multiple tries of memory access";
            return true;
        }
    }

    // If more than one operator
    if (GetCharacterCount(line, kOperators) > 1) {
        error_message = "Multiple arithmetic operators";
        return true;
    }

    return false;
}

int AssemblyParser::GetCharacterCount(string str, string to_find) {
    int result = 0;
    for (int i =0; i < str.length(); i++) {
        if (to_find.find(str[i]) != -1) result++;
    }

    return result;
}

void AssemblyParser::TranslateFunctionCalls(vector<int> &function_call_lines, vector<string> &result, map<string, int> &functions) {
    // Replace Function Calls with Function Line Addresses
    string line;
    for (int i = 0; i < function_call_lines.size(); i++) {
        int function_call_line = function_call_lines[i];
        line = result[function_call_line];
        int brace_o_idx = line.find(kFunctionBraceOpen);
        string func_name = line.substr(brace_o_idx + 1, line.length() - brace_o_idx - 2);
        line.replace(brace_o_idx, func_name.length() + 2, to_string(functions[func_name]));
        result[function_call_line] = line;
    }
}

void AssemblyParser::IllegalCodeError(int line, string error_message) {
    my_file.close();
    error = true;
    error_message += " at PC = " + to_string(line * 4) + " :: line " +  to_string(line);
    throw error_message;
}