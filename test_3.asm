R1 = 1
R2 = 2
R3 = 3
R4 = 4
R5 = 1
R6 = 0
BGE R5, 5, PC + 28
R6 = R6 + R1
R6 = R6 + R2
R6 = R6 + R3
R6 = R6 + R4
R5 = R5 + 1
JUMP PC - 24
RV = R6
RET