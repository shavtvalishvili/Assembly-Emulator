#ifndef registers_class
#define registers_class

using namespace std;

class Registers {
    public:
        Registers();
        ~Registers();
		int GetSP();
		void SetSP(int value);
		int GetPC();
		void SetPC(int value);
		int GetRV();
		void SetRV(int value);
		int Load(int register_idx);
		void Save(int register_idx, int value);

    private:
        // Reallocates kRegistersExpander(from "Constants.hpp") times more amount of memory to registers array
        void ExpandRegisters();

        // Array for general registers
        int* R;

        // Variable to store program cursor value, i.e. code line address
        int PC;

        // Variable to store stack pointer value
        int SP;

        // Variable to store return value
        int RV;
        
        // Variable to store the maximum number of registers consumer can use without register array reallocation
        int R_log_len;
};

#endif