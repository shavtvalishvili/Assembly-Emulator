#ifndef assembly_parser_class
#define assembly_parser_class

#include <string>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

class AssemblyParser {
    public:
        AssemblyParser();
        ~AssemblyParser();
        
        // Reads a file with a given name and returns a vector containing processed lines of assembly code
        // If errors are encountered while reading the file and analysing the code error message is thrown
        vector<string> GetVector();

        // Open file with a given filename and handle corresponding errors
        void InitializeParser(string filename);

		bool ErrorOccured();
        
    private:
        // Regular string trim
        void Trim(string &str);

        // Replaces multiple spaces with single space
        void RemoveMultipleSpaces(string &line);

        // Removes excess spaces from memory access instruction
        void CorrectMemoryAccessBraces(string &line);

        // If there are no spaces already, adds spaces before and after first occurances of symbols in the line
        void AddNecessarySpaces(string &line, string symbols);

        // Replaces constant negative sign string with minus sign and makes sure its directly followed by some value
        void TranslateNegativeValueSign(string &line);

        // Replaces function names with their line addressess in lines with function calls
        void TranslateFunctionCalls(vector<int> &function_call_lines, vector<string> &result, map<string, int> &functions);

        // Checks for a few illegalities in the given line and returns true if it comes across any of them
        bool IllegalCode(string line, string &error_message);

        // Returns how many times any character from "to_find" string occurs in the "str" string
        int GetCharacterCount(string str, string to_find);

        // Constructs a specific error message throws an error
        void IllegalCodeError(int line, string error_message);

		// Ifstream object, accessible from every function of this class
        ifstream my_file;
        
		// Variable used for indicating an unsuccessful opening of a file
        bool error;
};

#endif