#include <stdio.h>
#include <ucontext.h>

static ucontext_t uctx_main, uctx_func1, uctx_func2;

void func1() {
    printf("func1: started\n");
    printf("func1: swap to ctx_func2\n");
    if( swapcontext(&uctx_func1, &uctx_func2)<0 ) {
        perror("func1: swapcontext error\n");
        return;
    }
    printf("func1: swap back, continue \n");
    printf("func1: returning\n");
}

void func2() {
    printf("func2: started\n");
    printf("func2: swap to ctx_func1\n");
    if( swapcontext(&uctx_func2, &uctx_func1)<0 ) {
        perror("func2: swapcontext error\n");
        return;
    }
    printf("func2: swap back, continue \n");
    printf("func2: returning\n");
}

//   main 
//   -(switch)-> func2 
//   -(switch)-> func1 
//   -(switch)-> func2 
//   -(successor)-> func1 
//   -(successor)-> main
//   exit
//
//  main: swap to ctx_func2
//  func2: started
//  func2: swap to ctx_func1
//  func1: started
//  func1: swap to ctx_func2
//  func2: swap back, continue
//  func2: returning
//  func1: swap back, continue
//  func1: returning
//  main: swap back, continue
//  main: exiting
//
int main(int argc, char *argv[])
{
    char stack1[16*1024], stack2[16*1024];

    // Init uctx_func1
    if( getcontext(&uctx_func1)<0 ) {
        perror("getcontext error\n");
        return 1;
    }
    uctx_func1.uc_stack.ss_sp = stack1;
    uctx_func1.uc_stack.ss_size = sizeof(stack1);
    uctx_func1.uc_link = &uctx_main;
    makecontext(&uctx_func1, func1, 0);

    // Init uctx_func2
    if( getcontext(&uctx_func2)<0 ) {
        perror("getcontext error\n");
        return 1;
    }
    uctx_func2.uc_stack.ss_sp = stack2;
    uctx_func2.uc_stack.ss_size = sizeof(stack2);
    uctx_func2.uc_link = &uctx_func1;
    makecontext(&uctx_func2, func2, 0);

    printf("main: swap to ctx_func2\n");
    if( swapcontext(&uctx_main, &uctx_func2)<0 ) {
        perror("main: swapcontext error\n");
        return 1;
    }
    printf("main: swap back, continue \n");
    printf("main: exiting\n");
        
    return 0;
}
