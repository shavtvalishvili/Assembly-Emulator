#include "Registers.h"
#include "Constants.hpp"
#include <limits.h>

Registers::Registers() {
    R = (int*) malloc((kNumRegisters + 1) * sizeof(int));
    R_log_len = kNumRegisters;

    SP = INT_MAX;
    PC = 0;
}

Registers::~Registers() {
    free(R);
}

// Public

int Registers::Registers::GetSP() {
	return SP;
}

void Registers::Registers::SetSP(int value) {
	SP = value;
}

int Registers::GetPC() {
	return PC;
}

void Registers::SetPC(int value) {
	PC = value;
}

int Registers::GetRV() {
	return RV;
}

void Registers::SetRV(int value) {
	RV = value;
}

int Registers::Load(int register_idx) {
	if (register_idx > R_log_len) throw "Accessing uninitialized register";
	return R[register_idx - 1];
}

void Registers::Save(int register_idx, int value) {
	while (register_idx > R_log_len) ExpandRegisters();
	R[register_idx - 1] = value;
}

// Private

void Registers::ExpandRegisters() {
    R = (int*)realloc(R, (R_log_len * kRegistersExpander) * sizeof(int));
    R_log_len *= kRegistersExpander;
}