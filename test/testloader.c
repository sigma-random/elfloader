#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>


typedef int(*add_f)(int, int);  
        
add_f  add   = NULL;              // func pointer
int   *p_sum = NULL;

void *handle = NULL;

int main() {

    handle = (void*)dl_open("demolib.so", 0);
    if(NULL == handle){
        exit(EXIT_FAILURE);
    }

    /* find the global variable "g_sum" */
    p_sum = (int*)dl_sym(handle,"g_sum");
    if(NULL == p_sum) {
        printf("not found symbol: g_sum\n");
        exit(EXIT_FAILURE);
    }
    printf( "<g_sum> &g_sum = 0x%08x     g_sum = %d\n\n", p_sum, *p_sum);
    
    /* find a func named "g_add" */
    add = (add_f)dl_sym(handle,"g_add");
    if(NULL == add) {
        printf("not found symbol: g_add\n");
        exit(EXIT_FAILURE);
    }
    /* func add will change the value of g_sum */
    printf("<g_add> add(100, 200) = %d\n\n", add(100,200));
    /* output value of the global variable "g_sum" */
    printf( "<g_sum> &g_sum = 0x%08x     g_sum = %d\n\n", p_sum, *p_sum);

    dl_close(handle);

    return 0;
}

