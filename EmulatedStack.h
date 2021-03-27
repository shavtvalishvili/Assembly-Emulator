 /*
  *  EmulatedStack is an imitation of stack memory. If we look at stack memory as we did on seminars and
  *  lectures, we can say that EmulatedStack is a stack, that is rotated counterclockwise 90 degrees
  */

#ifndef emulated_stack_class
#define emulated_stack_class

using namespace std;

class EmulatedStack {
    public:
        EmulatedStack();
        ~EmulatedStack();

        // Writes first num_bytes amount of bytes of data in the address_space at a given address
        void WriteAt(int dest_address, int data, int num_bytes);

        // Reads 4 bytes from a given address from address_space
        int ReadAt(int address);
        
    private:
        // Reallocates kAddressSpaceExpander(from "Constants.hpp") times more amount of memory to address_space array
        void ExpandAddressSpace(char* &address_space, int &address_space_size);

        // Array of bytes which replicates stack memory, i.e. address space
        char* address_space;
        
        // Variable which stores size of address_space array
        int address_space_size;
};

#endif