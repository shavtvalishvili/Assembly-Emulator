#include <iostream>
#include <string>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <stack>
#include "Constants.hpp"
#include "EmulatedStack.h"
#include "Registers.h"
#include "AssemblyParser.h"

using namespace std::chrono;

/* Functions */

// Emulates processed text line by line
void Emulate(vector<string> &parsed_data, bool print_each_line, Registers &registers, EmulatedStack &emulated_stack);

// Emulates lines with equation
void EmulateEquation(string &line, int eq_idx, Registers &registers, EmulatedStack &emulated_stack);

// Emulates branch statements
void EmulateBranch(string &line, bool &jump, int &jump_dest, Registers &registers, EmulatedStack &emulated_stack);

// Emulates return statements of functions
void EmulateFunctionReturn(bool &jump, int &jump_dest, int &line_idx, int n_lines, Registers &registers, EmulatedStack &emulated_stack);

// In a string, gets numeric data from registers, performs an arithmetic operation on operands,
// reads 4 bytes from an address if necessary and returns the answer
int Evaluate(string str, Registers &registers, EmulatedStack &emulated_stack);

// From a string, gets information about how many bytes to read and write,
// erases that information from the string and returns the answer
int BytesToTake(string &line);

// Only takes first num_bytes amount of bytes from a passed
// integer and returns it as an integer
int TakeBytes(int answer, int num_bytes);

// Changes loop counter variable so that code emulation continues from a given line address
void JumpTo(int jump_dest, int &line);

// In a string, replaces register symbols with their numeric values
void ReplaceRegister(string &str, Registers &registers);

// In a string, extracts the encountered numbers and stores them into n1 and n2 respectively
void TakeOperands(string &str, int &n1, int &n2);

// Prints an error message if given, then draws a beautiful flower which freezes the program with its purity
void Freeze(string error = "");

/* Code */

int main() {
    try {
        // Parse input file into a vector
        AssemblyParser assembly_parser;
        bool print_each_line;

        cout << endl;
        do {
            string filename; cout << "Enter Filename: "; cin >> filename;
            
            assembly_parser.InitializeParser(filename);
        } while(assembly_parser.ErrorOccured());
        cout << "To Print Each Line Enter 1, Otherwise Enter 0: "; cin >> print_each_line; cout << endl;

        vector<string> parsed_data = assembly_parser.GetVector();

        // Memory
        EmulatedStack emulated_stack;
        
        // Registers
        Registers registers;

        // Saved PC Value of a Main Function to Quit the Program
        registers.SetSP(registers.GetSP() - 4);
        emulated_stack.WriteAt(registers.GetSP(), kMainSavedPCAddress, 4);


        auto start = high_resolution_clock::now();
        Emulate(parsed_data, print_each_line, registers, emulated_stack);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        cout << endl << "[Program duration: " << duration.count() << " milliseconds]" << endl;
    } catch (string error_message) {
        Freeze(error_message);
    }

    Freeze("");

    return 0;
}

void Emulate(vector<string> &parsed_data, bool print_each_line, Registers &registers, EmulatedStack &emulated_stack) {
    // Input Stream Storage Variable
    string line;

    // Emulation

    // For stack pointer monitoring to detect when it is not returned to its initial value at the end of functions
    stack<int> sp_monitoring; sp_monitoring.push(registers.GetSP());

    int size = parsed_data.size();
    for (int i = 0; i < size; i++) {

        registers.SetPC(i * 4);
        line = parsed_data[i];
        if (print_each_line) cout << registers.GetPC() << " :: " << line << endl;

        // Equation
        int eq_idx = line.find('=');
        if (eq_idx != -1) {
            EmulateEquation(line, eq_idx, registers, emulated_stack);

        // Branch or Jump
        } else {
            bool jump = true;
            int jump_dest;
            
            // Case Branch
            if (line[0] == kBranchPrefix) {
                EmulateBranch(line, jump, jump_dest, registers, emulated_stack);
            }
            // Case Jump
            else if (line.find(kJump) != -1) {
                jump_dest = Evaluate(line.substr(line.find(' ') + 1), registers, emulated_stack);
            }
            // Function Call
            else if (line.find(kCall) != -1) {
                registers.SetSP(registers.GetSP() - 4);
                emulated_stack.WriteAt(registers.GetSP(), registers.GetPC() + 4, 4);
                sp_monitoring.push(registers.GetSP());
                jump_dest = Evaluate(line.substr(line.find(' ') + 1), registers, emulated_stack);
            }
            // Return
            else if (line.find(kReturn) != -1) {
                if (registers.GetSP() != sp_monitoring.top()) {
                    string error_message = "Improper deallocation, SP not returned to its initial value";
                    throw error_message;
                }
                sp_monitoring.pop();
                EmulateFunctionReturn(jump, jump_dest, i, size, registers, emulated_stack);
            }
            if (jump) JumpTo(jump_dest, i);
        }
    }
}

void EmulateEquation(string &line, int eq_idx, Registers &registers, EmulatedStack &emulated_stack) {
    // Bytes to Take
    int num_bytes = BytesToTake(line); // 4 by default, only taken into account when handling first half of the equation

    // Split in Half
    string f_half = line.substr(0, eq_idx - 1);
    string s_half = line.substr(eq_idx + 2);
    
    // Evaluate Second Half
    int s_half_eval = Evaluate(s_half, registers, emulated_stack); // Byte Cutting Still Needed

    // First Half Cases
    // RV
    if (f_half.substr(0, kReturnValue.length()) == kReturnValue) {
        registers.SetRV(TakeBytes(s_half_eval, num_bytes));
    }
    // Registers
    else if (f_half.substr(0, kRegisterPrefix.length()) == kRegisterPrefix) {
        int idx = stoi(f_half.substr(kRegisterPrefix.length(), f_half.find(' ') - 1));
        registers.Save(idx, TakeBytes(s_half_eval, num_bytes));
    }
    // SP
    else if (f_half.substr(0, kStackPointer.length()) == kStackPointer) {
        registers.SetSP(TakeBytes(s_half_eval, num_bytes));
    }
    // M
    else if (f_half.substr(0, kMemoryAccess.length()) == kMemoryAccess) {
        int dest_address = Evaluate(f_half.substr(kMemoryAccess.length() + 1, f_half.length() - kMemoryAccess.length() - 2),
                                                                                                  registers, emulated_stack);
        emulated_stack.WriteAt(dest_address, s_half_eval, num_bytes);
    }
}

void EmulateBranch(string &line, bool &jump, int &jump_dest, Registers &registers, EmulatedStack &emulated_stack) {
    string f_word = line.substr(0, line.find(' ')); // Branch Keyword
    line = line.substr(kBranchLessThan.length() + 1);

    int n1 = Evaluate(line.substr(0, line.find(',')), registers, emulated_stack);
    line = line.substr(line.find(',') + 2);

    int n2 = Evaluate(line.substr(0, line.find(',')), registers, emulated_stack);
    line = line.substr(line.find(',') + 2);

    jump_dest = Evaluate(line, registers, emulated_stack);
    if (!f_word.compare(kBranchLessThan)) {
        jump = n1 < n2;
    } else if (!f_word.compare(kBranchLessEqual)) {
        jump = n1 <= n2;
    } else if (!f_word.compare(kBranchEquals)) {
        jump = n1 == n2;
    } else if (!f_word.compare(kBranchNotEquals)) {
        jump = n1 != n2;
    } else if (!f_word.compare(kBranchGreaterThan)) {
        jump = n1 > n2;
    } else if (!f_word.compare(kBranchGreaterEqual)) {
        jump = n1 >= n2;
    }
}

void EmulateFunctionReturn(bool &jump, int &jump_dest, int &line_idx, int n_lines, Registers &registers, EmulatedStack &emulated_stack) {
    string read_sp = kMemoryAccess + kMemoryAccessOpen + kStackPointer + kMemoryAccessClose;
    jump_dest = Evaluate(read_sp, registers, emulated_stack);
    if (jump_dest == -1) {
        jump = false;
        cout << "\n> Returned Answer: " << registers.GetRV() << endl;
        line_idx = n_lines;
    } else {
        registers.SetSP(registers.GetSP() + 4);
    }
}

int Evaluate(string str, Registers &registers, EmulatedStack &emulated_stack) {

    // Detect memory access or storing and remove corresponding symbols from string
    int m_idx = str.find(kMemoryAccess);
    if (m_idx != -1) {
        str = str.substr(kMemoryAccess.length() + 1, str.length() - kMemoryAccess.length() - 2);
    }

    // Evalutate SP or R1 or RV or PC
    if (str.find(kStackPointer) != -1) {
        str.replace(str.find(kStackPointer), kStackPointer.length(), to_string(registers.GetSP()));
    }
    if (str.find(kReturnValue) != -1) {
    	str.replace(str.find(kReturnValue), kReturnValue.length(), to_string(registers.GetRV()));
    }
    while (str.find(kRegisterPrefix) != -1) {
        ReplaceRegister(str, registers);
    }
    if (str.find(kProgramCursor) != -1) {
        str.replace(str.find(kProgramCursor), kProgramCursor.length(), to_string(registers.GetPC()));
    }

    // Take Operands
    int n1 = 0, n2 = 0;
    TakeOperands(str, n1, n2);

    // Store Operator Index if Present and in this Case Perform Arithmetic Operation on Operands
    int answer = n1;
    if (str.find('+') != -1) {
        answer = n1 + n2;
    } else if (str.find("- ") != -1) {
        answer = n1 - n2;
    } else if (str.find('*') != -1) {
        answer = n1 * n2;
    } else if (str.find('/') != -1) {
        answer = n1 / n2;
    }

    // M   
    if (m_idx != -1) answer = emulated_stack.ReadAt(answer);
    
    // Final Answer
    return answer;
}

int BytesToTake(string &line) {
    int result = 4;
    int dot_idx = line.find(kByteCutterOperator);
    if (dot_idx != -1) {
        result = line[dot_idx + 1] - '0';
        line.erase(dot_idx, 2);
    }

    return result;
}

int TakeBytes(int answer, int num_bytes) {
    if (num_bytes == 4) return answer;
    string byte_cutter = "";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            if (i < 4 - num_bytes) byte_cutter += '0';
            else byte_cutter += '1';
        }
    }
    int binary_cutter = stoi(byte_cutter, nullptr, 2);
    return answer & binary_cutter;
}

void JumpTo(int jump_dest, int &line) {
    line = jump_dest / 4 - 1;
}

void ReplaceRegister(string &str, Registers &registers) {
    string num;
    int r_idx = str.find(kRegisterPrefix);
    int space = str.find(' ', r_idx);
    if (space != -1) {
        num = str.substr(r_idx + 1, space - r_idx - 1);
    } else {
        num = str.substr(r_idx + 1);
        if (num[num.length() - 1] == kMemoryAccessClose) {
        	num = num.substr(0, num.length() - 1);
    	}
    }
    str.replace(r_idx, num.length() + 1, to_string(registers.Load(stoi(num))));
}

void TakeOperands(string &str, int &n1, int &n2) {
	// If a number is followed by a non-digit character it's assumed that
	// it's also followed by an arithmetic operator and the second operand
    int counter = str.find_first_of(kDigits);
    bool negative = false;
    if (counter - 1 >= 0) negative = str[counter - 1] == '-';
    int not_digit_idx = str.find_first_not_of(kDigits, counter);
    n1 = stoi(str.substr(counter, not_digit_idx ? -1 -1 : (not_digit_idx - counter)));
    if (negative) n1 = -n1;
    if (not_digit_idx != -1) {
        counter = str.find_first_of(kDigits, not_digit_idx);
        negative = (str[counter - 1] == '-');
        n2 = stoi(str.substr(counter, not_digit_idx ? -1 -1 : (not_digit_idx - counter)));
        if (negative) n2 = -n2;
    }
}

void Freeze(string error) {
    cout << endl;
    if (error != "") cout << "<< ! Error: " << error << " ! >>" << endl << endl;
    cout << "               __/)\n            .-(__(=:\n         |\\ |    \\\n         \\ ||)" << endl;
    cout << "          \\||\n           \\|\n> Stunned by|beauty," << endl;
    cout << "> Program wi|l now finish execution and freeze forever.";
    std::this_thread::sleep_for(std::chrono::milliseconds(1500)); cout << " .";
    std::this_thread::sleep_for(std::chrono::milliseconds(1500)); cout << "  ." << endl;
    while(true);
    cout << "> For you to read results nicely" << endl;
}