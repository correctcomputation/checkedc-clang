// RUN: CPATH=$CHECKED_CPATH:$CPATH cconv-standalone %s -- | FileCheck -match-full-lines %s

#include<signal_checked.h>

void vsf_sysutil_set_sighandler(int sig, void (*p_handlefunc)(int))
{
    int retval;
    struct sigaction sigact;
    sigact.sa_handler = p_handlefunc;
} 
//CHECK: void vsf_sysutil_set_sighandler(int sig, void (*p_handlefunc)(int))

/*ensure trivial conversion*/
void foo(int *x) { 

}