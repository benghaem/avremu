#include "memory.h"
#include <stdint.h>

typedef enum{
    ADD,
    ADIW,
    AND,
    ANDI,
    ASR,
    BCLR,
    BLD,
    BRBC,
    BRBS,
    BREAK,
    BSET,
    BST,
    CALL,
    CBI,
    COM,
    CP,
    CPC,
    CPI,
    CPSE,
    DEC,
    DES,
    EICALL,
    EIJMP,
    ELPM1,
    ELPM2,
    ELPM3,
    EOR,
    FMUL,
    FMULS,
    FMULSU,
    ICALL,
    IJMP,
    IN,
    INC,
    JMP,
    LAC,
    LAS,
    LAT,
    LD1,
    LD2,
    LD3,
    LDD_4,
    LDD_Y2,
    LDD_Y3,
    LDD_Z2,
    LDD_Z3,
    LDD_Z4,
    LDI,
    LDS,
    LPM1,
    LPM2,
    LPM3,
    LSR,
    MOV,
    MOVW,
    MUL,
    MULSU,
    NEG,
    NOP,
    OR,
    ORI,
    OUT,
    POP,
    PUSH,
    RCALL,
    RET,
    RETI,
    RJMP,
    ROL,
    ROR,
    SBC,
    SBCI,
    SBI,
    SBIC,
    SBIS,
    SBIW,
    SBRC,
    SBRS,
    SLEEP,
    SPM,
    SPM2_4,
    ST1,
    ST2,
    ST3,
    STS,
    ST_Y2,
    ST_Y3,
    ST_Y4,
    ST_Z2,
    ST_Z3,
    ST_Z4,
    SUB,
    SUBI,
    SWAP,
    WDR,
    XCH,
    UNKNOWN,
} INSTRUCTION;

INSTRUCTION INSTRUCTION_decode_bytes(uint16_t bytes);

int INSTRUCTION_is_32b(INSTRUCTION instr);

void INSTRUCTION_print(INSTRUCTION instr);

typedef struct
{

    INSTRUCTION inst;
    uint16_t bits;
    uint16_t ex_bits;

} OPERATION;
