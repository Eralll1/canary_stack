# Canary Stack
Simple stack implementation with canary protection

## Compilation

You can compile it with 
> gcc stack_canary.c mylib.c -o main

## Usage

Type of items in stack is defined as "stack_t" in 'stack_canary.h' and can be changed before compile

All stack relatated methods are taking int* as status and sets bits for different problems

ST_OK               = 0    ~ everything is fine <br>
ST_left_cn_damaged  = 1<<0 ~ stack underflew <br>
ST_right_cn_damaged = 1<<1 ~ stack overflew <br>
ST_invalid_cur_size = 1<<2 ~ stack top is invalid <br>
ST_invalid_max_size = 1<<3 ~ stack capacity is invalid <br>
ST_invalid_mem_ptr  = 1<<4 ~ start of stack is invalid pointer <br>
ST_NULL_mem_ptr     = 1<<5 ~ start of stack is NULL <br>
ST_calloc_fail      = 1<<6 ~ calloc fialed <br>
ST_invalid_stack    = 1<<7 ~ not stack* was passed <br> 
ST_empty_pop        = 1<<8 ~ tried popping an empty stack <br>

stack_create take a pointer to an uninitialized struct Stack
stack capacity is locked to a power of 2 

if status ptr is invalid programm aborts as it has no way to persue
functions that are denoted with '_' are private

## Security

Every method has multiple checks if the stack is valid including 
checking whether 2 canaries are 'alive' 




// TODO 
// binary with separators
// lowe rand upper case letters (error string)
// strerr 