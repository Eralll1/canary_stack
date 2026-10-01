#include "../mylib.h"

typedef int stack_t;
#define ST_STACKSIZE 5

#define CANARY_AMOUNT 2
#define CANARY_VAL 234567
#define DEFAULT_VAL -1

enum Stack_Exceptions {
    ST_OK = 0,
    ST_left_cn_damaged  = 1<<0,
    ST_right_cn_damaged = 1<<1,
    ST_invalid_cur_size = 1<<2,
    ST_invalid_max_size = 1<<3,
    ST_invalid_mem_ptr  = 1<<4,
    ST_NULL_mem_ptr     = 1<<5,
    ST_calloc_fail      = 1<<6,
    ST_invalid_stack    = 1<<7,
    ST_empty_pop        = 1<<8,
};

struct Stack {
    stack_t* mem_start;
    size_t cur_size;
    size_t max_size;
};



void dump_stack(FILE* output, struct Stack* stack, int* status);

#define print_stack_to_stderr(stack) dump_stack(stdout, stack, NULL)


#define CHECK_STACK(stack, status, ret) do {  \
    assert(status != NULL); \
    *status |= _stack_validate(stack); \
    if (*status != ST_OK) {dump_stack(stderr, stack, status); return ret;} \
} while (0)

#define CHECK_STACK_VOID(stack, status) do {  \
    assert(status != NULL); \
    *status |= _stack_validate(stack); \
    if (*status != ST_OK) {dump_stack(stderr, stack, status); return;} \
} while (0)

#define CHECK_STACK_EXIST(stack, status, ret) do { \
    assert(status != NULL); \
    if(stack == NULL) { \
        *status |= ST_invalid_stack; \
        dump_stack(stderr, stack, status); \
        return ret; \
    } \
} while(0)



// Безобразно зато единообразно)

// TODO: push->stack_push et cetera      +
// TODO: Reorder functions               +
// TODO: implement stack_free            +
// TODO: mb create_stack -> stack_create +

int         _stack_validate             (struct Stack* stack);
stack_t*    _get_top_ptr          (struct Stack* stack, int* status);
stack_t*    _get_canary_at_end    (struct Stack* stack, int* status);

void        stack_create          (struct Stack* stack, int* status, size_t elem_count);
void        stack_resize          (struct Stack* stack, int* status, size_t new_size);
void        stack_push            (struct Stack* stack, int* status, stack_t elem);
stack_t     stack_pop             (struct Stack* stack, int* status);
void        stack_free            (struct Stack* stack, int* status);



