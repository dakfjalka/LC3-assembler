#ifndef _INSTRUCTION_HANDLER_H
#define _INSTRUCTION_HANDLER_H
#include "assembler.h"
#include <stdint.h>

typedef _err_type (*_instruction_handler_type)(_line_type *, _label_list_type *, _machine_code_type *);
typedef _err_type (*_pseudo_op_handler_type)(_line_type *, _label_list_type *, _machine_code_list_type *);


_err_type ADD_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type AND_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type BR_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type JMP_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type JSR_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type LD_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type LDI_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type LDR_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type LEA_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type NOT_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type RET_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type RTI_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type ST_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type STI_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type STR_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type TRAP_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);
_err_type HALT_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_type *machine_code);

_err_type FILL_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_list_type *machine_code_list);
_err_type BLKW_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_list_type *machine_code_list);
_err_type STRINGZ_handler(_line_type *p_line, _label_list_type *label_list, _machine_code_list_type *machine_code_list);

_err_type base_address_handler(int base_address, _machine_code_list_type *machine_code_list);

#endif 