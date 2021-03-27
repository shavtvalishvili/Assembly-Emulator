# Assembly Emulator

Program takes a generalized Assembly language code as input, parses it and emulates its execution in the CPU in C++.

Program emulates stack and registers as separate classes. At every step needed space is allocated, memory registers and register values like stack pointer, program cursor, return value are stored and renewed. Finally, after line-by-line emulation program prints the return value.

**For code examples see test_N.asm (N for numbers 0-4) files included in the same directory**

----

### Syntax Details

Syntax that a file must contain in order for the program to function correctly follows handout examples of
assembly code from CS107 Programing Paradigms course of Stanford University, specifically:

#### You can use:
    1. Registers like SP for stack pointer, RV for return value, PC for program cursor;
    2. Memory registers like R1, R2... (kNumRegisters in total, defined in "Constants.hpp")
    3. Statements like RET for function return, JUMP for jump to a specific line in code,
       CALL for function calls, branch statements like BEQ, BGE and etc.,
       M[X] for accessing memory from address (where X is an address represented by an integer,
       register or an arithmetic operation without rule violations given below)
    4. Arithmetic operators like +, -, *, /, or unary byte cutter operator .N for specifying the
       number of bytes to take from the operand where N is the number of bytes;
    (These entities are defined in "Constants.hpp" as constants and can be modified)

#### You can:
    1. Write comments;
    2. Write unlimited newlines between code lines;
    3. Write arithmetic operators right before and right after operands;

#### Additionally, you can:
    1. Put negative signs to constants and registers by writing kNegativeSign constant string defined in 
      "Constants.hpp" before them;
    2. Write as many spaces and tabs you like wherever you like;

### Rule Violations

Below are rule violations in our generalized Assembly language.
Program checks input code for these violations and in case of a violation it prints errors.
#### Rule violations:
    1. A code line contains more than just one LOAD or STORE instruction;
    2. A code line contains more than one arithmetic operator;
    3. Code doesn't deallocate allocated memory for variablers at the end of a function;

### Notes

#### Note that:
    1. Function call statement accepts function names with corresponding braces defined in "Constants.hpp" or line addresses;
    2. Except for the main function, function description must start with a seperate line containing function name with
       corresponding braces defined in "Constants.hpp" and description must be finished by return statement;
    3. Address space, i.e. stack memory is bounded by INT_MAX as int is used as a "memory address pointer"
       as well as address space size variable;
    4. Every register can store 4 bytes(sizeof(int)) of data;

### Build instructions:
In this directory run:
```cmd
> g++ *.cpp
```

**For code examples see test_N.asm (N for numbers 0-4) files included in the same directory**
