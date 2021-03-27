#ifndef assembly_emulator_constants
#define assembly_emulator_constants

#include <string>
using namespace std;

// Constants

const string kOperators = "+-*/";
const char kByteCutterOperator = '.';

const int kInitAddressSpaceSize = 1024; // 1KB
const double kAddressSpaceExpander = 2;

const int kNumRegisters = 32; // 32 Registers
const double kRegistersExpander = 2;

// Initial Saved PC Value for Quitting the Program
const int kMainSavedPCAddress = -1;

const string kDigits = "0123456789";

// Unique Character for Commenting
const char kCommentChar = ';';

// Keyword Constants

// Negative value sign
const string kNegativeSign = "NEG";

// Every Keyword Must Be Unique
const string kStackPointer = "SP";

const string kProgramCursor = "PC";

const string kRegisterPrefix = "R";

const string kReturnValue = "RV";

const string kMemoryAccess = "M";
const char kMemoryAccessOpen = '[';
const char kMemoryAccessClose = ']';

// First Letter of Every Branch Keyword Indicates that the word is a Branch Statement
const char kBranchPrefix = 'B';

const string kBranchLessThan = "BLT";       // Branch Less Than
const string kBranchLessEqual = "BLE";      // Branch Less Than or Equal
const string kBranchEquals = "BEQ";         // Branch Equal
const string kBranchNotEquals = "BNE";      // Branch Not Equal
const string kBranchGreaterThan = "BGT";    // Branch Greater Than
const string kBranchGreaterEqual = "BGE";   // Branch Greater Than or Equal

const string kJump = "JUMP";
const string kReturn = "RET";
const string kCall = "CALL";

#endif