SP = SP - 8
M[SP + 4] = NEG180
M[SP] = 7
CALL <remainder>
M[SP + 4] = RV
M[SP] = 0
R1 = M[SP]
BGE R1, 1000, PC + 32
R1 = M[SP     +     4] ; For excess space removal demonstration
R1 = R1 + 1
M[SP + 4]       =   R1 ; For excess tab and space removal demonstration
R1 = M[SP]
R1 = R1 + 1
M[SP] = R1
JUMP PC - 32
RV = M[SP + 4]
SP = SP + 8
RET

    ; This function is a very well-behaved function
    <remainder>
    R1 = M[   SP + 8]
    R2 = M[SP + 4]
    R3 = R1 / R2
    R2 = R2 * R3
    RV = R1 - R2
    RET