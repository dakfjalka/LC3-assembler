#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

/**
 * @brief 定义了一些常量，便于修改和 debug 罢了
 * 
 */
#define INSTRUCTION_STRING_LEN  6
#define PSEUDO_OPS_STRING_LEN   10
#define NUM_OF_LINES            128
#define LABEL_NAME_SIZE         10
#define NUM_OF_LABELS           150
#define LINE_WORD_LEN           10
#define LINE_WORD_NUM           6
#define LC3_INSTRUCTION_LEN     16
#define LC3_INSTRUCTION_STR_LEN 17


/**
 * @brief 枚举型变量错误信息，里面饱和了我到现在为止能想到的各种错误。
 * 
 */
typedef enum{
    SUCCESS,
    FAIL,
    DOUBLE_ORIG,
    NO_ORIG_FOUND,
    ORIG_INVALID_ADDR,
    NO_END_FOUND,
    INVALID_WORD,
    INAVLID_INSTRUCTION,
    INVALID_REGISTER,
    INVALID_IMMIDIATE_VALUE,
    IMM_VALUE_TO_STRING_FAIL,
    LABEL_UNDEFINED,
    OFFSET_TO_STRING_FAIL,
    INVALID_TRAP_VECTOR,
    INVALID_BASE_ADDRESS,
    CREAT_OUTPUT_FILE_FAIL,
    STRING_FAIL
}__err_info_type;


/**
 * @brief 错误结构体，目前包含错误出现的行数（源文件中的行）和错误信息。
 * 
 */
typedef struct{
    int line;                   // 在源文件第几行，如果不知道怎么描述就是第-1行
    __err_info_type info;       // 错误信息
}_err_type;


/**
 * @brief 源文件的每一行被储存在这样的结构体中。
 * 
 */
typedef struct _o_line_type{
    int physical_line;                              // 在 assemble 好的文件中它应当在第几行（注意不是地址）
    int origional_line;                             // 在源文件中它在第几行
    int words_num;                                  // 有多少个单词
    char content[LINE_WORD_NUM][LINE_WORD_LEN];     // 这一行的单词，不包含空格或换行符，但是包含 . 和 #
    struct _o_line_type *next;                      // 指向下一个的指针（没错这是链表）
}_line_type;


/**
 * @brief 文件中出现的所有的标签
 * 
 */
typedef struct _o_label_type{
    char name[LABEL_NAME_SIZE];     // 标签名称
    int address;                    // 注意是在 assemble 后相对于第一行的地址增量，不是真正的地址
    struct _o_label_type *next;     // 指向下一个的指针
}_label_type;


/**
 * @brief 翻译完成的机器码
 * 
 */
typedef struct _o_machine_code_type{
    char machine_code[LC3_INSTRUCTION_STR_LEN];     // 机器码
    struct _o_machine_code_type *next;              // 指向下一个的指针
}_machine_code_type;


/**
 * @brief 函数声明
 * {
 */
int __split_str(char *s_str, const char *split_char, char **d_str);
void __dete_comment(char *_str);
int __is_key_word(char *str);
int __raise_error(_err_type err);
int __get_const_value(char *str);
_err_type _pre_process(char *buffer, _line_type *line_list);
_err_type _first_path(_line_type *line_list, _label_type *label_list, int *base_address);
_err_type _second_process(_line_type *line_list, _label_type *label_list, int base_address, _machine_code_type *machine_code_list);
_err_type _save_machine_code_string(_machine_code_type *machine_code_list);
int assemble(char *buffer);
/**
 * }
*/

#endif  // __ASSEMBLER_H__
