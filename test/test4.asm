.ORIG x3000
    RTI
    RET
    RTI
    RET
    RTI

ST R1, label3

label1 .FILL xffff
LDR R1, R2, #-1
label2
.BLKW #5
LEA R4, label2
label3 .STRINGZ "HELLO"
BR label2
HALT
label4
.END