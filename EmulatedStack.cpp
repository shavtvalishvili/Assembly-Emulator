#include "EmulatedStack.h"
#include "Constants.hpp"
#include <iostream>
#include <limits.h>

// Public

EmulatedStack::EmulatedStack() {
    address_space = (char*) malloc(kInitAddressSpaceSize);
    address_space_size = kInitAddressSpaceSize;
}

EmulatedStack::~EmulatedStack() {
    free(address_space);
}

void EmulatedStack::WriteAt(int dest_address, int data, int num_bytes) {
    unsigned char* tmp = (unsigned char*)&data;
    int init = INT_MAX - dest_address - 1;
    while (init >= address_space_size) ExpandAddressSpace(address_space, address_space_size);
    for (int i = init; i > init - num_bytes; i--) {
        address_space[i] = tmp[init - i];
    }
}

int EmulatedStack::ReadAt(int address) {
    unsigned char tmp[4];
    *((int*)tmp) = 0;
    int init = INT_MAX - address - 1;
    while (init >= address_space_size) ExpandAddressSpace(address_space, address_space_size);
    for (int i = init; i > init - 4; i--) {
        if (i >= 0) tmp[init - i] = address_space[i];
    }
    return *((int*)tmp);
}

// Private

void EmulatedStack::ExpandAddressSpace(char* &address_space, int &address_space_size) {
    address_space = (char*)realloc(address_space, address_space_size * kAddressSpaceExpander);
    address_space_size *= kAddressSpaceExpander;
}
