#include "stack_canary.h"


// LONG TODO
// print status


// TODO
// document that memory is not freed if calloc fail     +
// check for invalid pointer                            +
// One more canary (at the very end )                   +
// dump MB                                              +


    // ST_left_cn_damaged  = 1<<0,
    // ST_right_cn_damaged = 1<<1,
    // ST_invalid_cur_size = 1<<2,
    // ST_invalid_max_size = 1<<3,
    // ST_invalid_mem_ptr  = 1<<4,
    // ST_NULL_mem_ptr     = 1<<5,
    // ST_calloc_fail      = 1<<6,
    // ST_invalid_stack    = 1<<7,
    // ST_empty_pop        = 1<<8,

void _print_status(FILE* output, struct Stack* stack, int* status) {

    if (*status == ST_OK) {
        fprintf(output, "Stack is fine\n");
        return;
    }

    fprintf(output, "status = | ");

    if (*status & ST_calloc_fail)       fprintf(output, "Calloc failed | ");
    if (*status & ST_invalid_stack)     fprintf(output, "invalid stack was passed | ");
    if (*status & ST_empty_pop)         fprintf(output, "Tryed popping empty stack | ");

    if (stack == NULL) {
        fprintf(output, "\n");
        return;
    }

    if (*status & ST_left_cn_damaged)   fprintf(output, "Left canary was damaged | ");
    if (*status & ST_right_cn_damaged)  fprintf(output, "Right canary was damaged | ");
    if (*status & ST_invalid_cur_size)  fprintf(output, "Cur_size is <%zu> | ", stack->cur_size);
    if (*status & ST_invalid_max_size)  fprintf(output, "Max_size is <%zu> | ", stack->max_size);
    if (*status & ST_invalid_mem_ptr)   fprintf(output, "Memo_start is <%p> | ", stack->mem_start);

    fprintf(output, "\n");

}

void dump_stack(FILE* output, struct Stack* stack, int* status) {

    int status_val = (status == NULL) ? ST_OK : *status;
    
    if (stack == NULL) {return;}

    fprintf(output, "=====================-- STACK --=====================\n");
    // fprintf(output, "%s\n", stack->created_history);
    // fprintf(output, "=====================-- PARAMS --====================\n");
    fprintf(output, "cur_size = %zu\n", stack->cur_size);
    fprintf(output, "max_size = %zu\n", stack->max_size);
    fprintf(output, "mem_start = [ %p ]\n", stack->mem_start);
    fprintf(output, "CANARY_VAL = [ %d ]\n", CANARY_VAL);
    _print_status(output, stack, status);

    
    if ((status_val & (ST_calloc_fail | ST_NULL_mem_ptr | ST_invalid_max_size | ST_invalid_mem_ptr)) == 0){
        fprintf(output, "=====================-- DATA --======================\n");
        for(int i = 0; i < stack->max_size + 2; i++){
            fprintf(output, "[%d] = <%d>", i, stack->mem_start[i]); // TODO %d depend on stack_t

            if (i == 0)
                fprintf(output, "\t\t<-- canary 1");
            if (i == stack->cur_size)
                fprintf(output, "\t\t<-- stack top");
            if (i == stack->cur_size + 1)
                fprintf(output, "\t\t<-- canary 2");

            fprintf(output, "\n");
        }
    }

    fprintf(output, "=====================================================\n");
}

void stack_free(struct Stack* stack, int* status){
    // CHECK_STACK_VOID(stack, status);
    if (stack == NULL || stack->mem_start == NULL) return;
    free(stack->mem_start);
    stack->mem_start = NULL;
}

// int is not understandable //DED
int _stack_validate(struct Stack* stack){
    // TODO: check the stack pointer itself (stack!=NULL) +
    int status = ST_OK;

    CHECK_STACK_EXIST(stack, &status, status);

    if (stack->mem_start == NULL){
        status |= ST_NULL_mem_ptr;
        return status;
    }
    if (stack->cur_size < 0 || stack->cur_size > stack->max_size){
        status |= ST_invalid_cur_size;
    }
    if (stack->max_size < 0){
        status |= ST_invalid_max_size;
    }
    if (*(stack->mem_start) != CANARY_VAL){
        status |= ST_left_cn_damaged;
    }
    // get_canary_at_end(stack,status)=(get_top_ptr(stack, status) + 1) + 
    if ((status & ST_invalid_cur_size) == 0 && (*_get_canary_at_end(stack, &status)) != CANARY_VAL) {
        status |= ST_right_cn_damaged;
    }

    return status;
}

stack_t* _get_canary_at_end(struct Stack* stack, int* status) {
    CHECK_STACK_EXIST(stack, status, NULL);
    return stack->mem_start + stack->cur_size +1;
}
stack_t* _get_top_ptr(struct Stack* stack, int* status){
    CHECK_STACK_EXIST(stack, status, NULL);
    return stack->mem_start + stack->cur_size;
}

// thanks to DIMA(lyasevich github.com/) for the idea // TODO
void stack_create(struct Stack* stack, int* status, size_t elem_count){

    // CHECK_STACK_RET(stack); // meaningless

    size_t st_size = max(elem_count, ST_STACKSIZE);
    size_t st_size_rounded = ceil_to_power_of_two(st_size);
    // size_t st_size_rounded = st_size;
    // accounted for 2 canaries
    // MB ask ded about calloc for dif size (if canaries are not the same size as stack_t) //DED
    stack_t* ptr = calloc(st_size_rounded + CANARY_AMOUNT, sizeof(stack_t));
    if (ptr == NULL) { 
        *status |= ST_calloc_fail;
        return;
    }

    *ptr = CANARY_VAL;
    *(ptr+1) = CANARY_VAL;

    stack->cur_size = 0;
    stack->max_size = st_size_rounded;
    stack->mem_start = ptr;

    CHECK_STACK_VOID(stack, status);
}

void stack_resize(struct Stack* stack, int* status, size_t new_size){

    CHECK_STACK_VOID(stack, status);

    size_t new_size_rounded = ceil_to_power_of_two(new_size);

    // do this to avoid uninitialized memory
    // though it should not matter
    // TODO: INCLUDE CANARY +
    stack_t* new_ptr = calloc(new_size_rounded + CANARY_AMOUNT, sizeof(stack_t));
    if (new_ptr == NULL) { 
        *status |= ST_calloc_fail;
        return;
    }
    memcpy(new_ptr, stack->mem_start, (stack->cur_size + CANARY_AMOUNT) * sizeof(stack_t));
    free(stack->mem_start);

    stack->max_size = new_size_rounded;
    stack->mem_start = new_ptr;

    CHECK_STACK_VOID(stack, status);
}

void stack_push(struct Stack* stack, int* status, stack_t elem){

    CHECK_STACK_VOID(stack, status);

    if (stack->cur_size >= stack->max_size) {
        stack_resize(stack, status, stack->cur_size+1);
    }

    CHECK_STACK_VOID(stack, status);

    stack_t* ptr_after_top = _get_canary_at_end(stack, status);

    CHECK_STACK_VOID(stack, status);

    *ptr_after_top = elem;
    *(ptr_after_top + 1) = CANARY_VAL; 

    stack->cur_size++;

    CHECK_STACK_VOID(stack, status);
}

stack_t stack_pop(struct Stack* stack, int* status) {
    // TODO: mb precheck cur_size (if stack->cur_size==0: *status = *status or ST_,,,; return ...) +
    CHECK_STACK(stack, status, DEFAULT_VAL);
    if (stack->cur_size <= 0) {
        *status |= ST_empty_pop;
        return DEFAULT_VAL;
    }

    stack_t* old_top = _get_top_ptr(stack, status);
    CHECK_STACK(stack, status, DEFAULT_VAL);
    stack_t res = *old_top;

    CHECK_STACK(stack, status, DEFAULT_VAL);

    memset(old_top+1, 0, sizeof(stack_t));
    // dump_stack(stderr, stack, status);
    stack_t temp = CANARY_VAL;
    memcpy(old_top, &temp, sizeof(stack_t));
    // dump_stack(stderr, stack, status);

    // CHECK_STACK(stack, status, DEFAULT_VAL);

    stack->cur_size--;

    CHECK_STACK(stack, status, DEFAULT_VAL);

    return res;
}


int main(){
    struct Stack stack = {};
    int status_val = ST_OK;
    int* status = &status_val;
    stack_create(&stack, status, 2);

    stack_resize(&stack, status, 12);
    stack_pop(&stack, status);
    dump_stack(stderr, &stack, status);
    //TODO: free + 
    
    stack_free(&stack, status);

}
