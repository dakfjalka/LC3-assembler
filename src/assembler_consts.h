/**
 * @file assembler_consts.h
 * @author PB21000293
 * @brief assembler.c 中需要的部分常量，为了好看就单独开了个文件。
 *        只能被 assembler.c 引用，否则会导致重复定义。
 * @version 0.3
 * @date 2022-12-14
 * 
 * @copyright 这是是啥？
 * 
 */

#ifndef __ASSEMBLER_CONSTS_H
#define __ASSEMBLER_CONSTS_H

#include "assembler.h"
#include "instruction_handler.h"

/**
 * @brief 指令字符串
 * 
 */
const char __instructions_string[][INSTRUCTION_STRING_LEN] = {
    "ADD",
    "AND",
    "BR",
    "BRn",
    "BRz",
    "BRp",
    "BRnz",
    "BRzp",
    "BRnzp",
    "JMP",
    "JSR",
    "JSRR",
    "LD",
    "LDI",
    "LDR",
    "LEA",
    "NOT",
    "RET",
    "RTI",
    "ST",
    "STI",
    "STR",
    "TRAP",
    "HALT"
};


/**
 * @brief 事实证明下面这一串我没有用
 * 
 */
const char __instructions[][5] = {
    "0001",
    "0101",
    "0000",
    "0000",
    "0000",
    "0000",
    "0000",
    "0000",
    "0000",
    "1100",
    "0100",
    "0100",
    "0010",
    "1010",
    "0110",
    "1110",
    "1001",
    "1100",
    "1000",
    "0011",
    "1011",
    "0111",
    "1111",
    "1111"
};


/**
 * @brief 伪指令（好像翻译是这样？）
 * 
 */
const char __pseudo_ops[][PSEUDO_OPS_STRING_LEN] = {
    ".FILL",
    ".BLKW",
    ".STRINGZ",
    ".ORIG",
    ".END"
};


/**
 * @brief 与指令对应的操作函数。注意一定要和前面定义的指令字符串相匹配！
 * 
 */
const _instruction_handler_type instruction_handler[] = {
    ADD_handler,
    AND_handler,
    BR_handler,
    BR_handler,
    BR_handler,
    BR_handler,
    BR_handler,
    BR_handler,
    BR_handler,
    JMP_handler,
    JSR_handler,
    JSR_handler,
    LD_handler,
    LDI_handler,
    LDR_handler,
    LEA_handler,
    NOT_handler,
    RET_handler,
    RTI_handler,
    ST_handler,
    STI_handler,
    STR_handler,
    TRAP_handler,
    HALT_handler,
};


/**
 * @brief 同上
 * 
 */
const _pseudo_op_handler_type pseudo_op_handler[] = {
    FILL_handler,
    BLKW_handler,
    STRINGZ_handler
};

#endif
