// RUN: %S/3c-regtest.py --predefined-script common-testgenerator %s -t %t --clang '%clang'


/*********************************************************************************/

/*This file tests three functions: two callers bar and foo, and a callee sus*/
/*In particular, this file tests: how the tool behaves when there is an array
of structs*/
/*For robustness, this test is identical to arrofstructboth.c except in that
a prototype for sus is available, and is called by foo and bar,
while the definition for sus appears below them*/
/*In this test, foo will treat its return value safely, but sus and bar will not,
through invalid pointer arithmetic, an unsafe cast, etc.*/

/*********************************************************************************/


#include <stddef.h>
extern _Itype_for_any(T) void *calloc(size_t nmemb, size_t size) : itype(_Array_ptr<T>) byte_count(nmemb * size);
extern _Itype_for_any(T) void free(void *pointer : itype(_Array_ptr<T>) byte_count(0));
extern _Itype_for_any(T) void *malloc(size_t size) : itype(_Array_ptr<T>) byte_count(size);
extern _Itype_for_any(T) void *realloc(void *pointer : itype(_Array_ptr<T>) byte_count(1), size_t size) : itype(_Array_ptr<T>) byte_count(size);
extern int printf(const char * restrict format : itype(restrict _Nt_array_ptr<const char>), ...);
extern _Unchecked char *strcpy(char * restrict dest, const char * restrict src : itype(restrict _Nt_array_ptr<const char>));

struct general { 
    int data; 
    struct general *next;
	//CHECK_NOALL: struct general *next;
	//CHECK_ALL: _Ptr<struct general> next;
};

struct warr { 
    int data1[5];
	//CHECK_NOALL: int data1[5];
	//CHECK_ALL: int data1 _Checked[5];
    char *name;
	//CHECK: _Ptr<char> name;
};

struct fptrarr { 
    int *values; 
	//CHECK: _Ptr<int> values; 
    char *name;
	//CHECK: _Ptr<char> name;
    int (*mapper)(int);
	//CHECK: _Ptr<int (int )> mapper;
};

struct fptr { 
    int *value; 
	//CHECK: _Ptr<int> value; 
    int (*func)(int);
	//CHECK: _Ptr<int (int )> func;
};  

struct arrfptr { 
    int args[5]; 
	//CHECK_NOALL: int args[5]; 
	//CHECK_ALL: int args _Checked[5]; 
    int (*funcs[5]) (int);
	//CHECK_NOALL: int (*funcs[5]) (int);
	//CHECK_ALL: _Ptr<int (int )> funcs _Checked[5];
};

int add1(int x) { 
	//CHECK: int add1(int x) _Checked { 
    return x+1;
} 

int sub1(int x) { 
	//CHECK: int sub1(int x) _Checked { 
    return x-1; 
} 

int fact(int n) { 
	//CHECK: int fact(int n) _Checked { 
    if(n==0) { 
        return 1;
    } 
    return n*fact(n-1);
} 

int fib(int n) { 
	//CHECK: int fib(int n) _Checked { 
    if(n==0) { return 0; } 
    if(n==1) { return 1; } 
    return fib(n-1) + fib(n-2);
} 

int zerohuh(int n) { 
	//CHECK: int zerohuh(int n) _Checked { 
    return !n;
}

int *mul2(int *x) { 
	//CHECK: _Ptr<int> mul2(_Ptr<int> x) _Checked { 
    *x *= 2; 
    return x;
}

struct general ** sus(struct general *, struct general *);
	//CHECK_NOALL: struct general **sus(struct general *x : itype(_Ptr<struct general>), struct general *y : itype(_Ptr<struct general>)) : itype(_Ptr<struct general *>);
	//CHECK_ALL: _Array_ptr<_Ptr<struct general>> sus(struct general *x : itype(_Ptr<struct general>), _Ptr<struct general> y);

struct general ** foo() {
	//CHECK_NOALL: _Ptr<struct general *> foo(void) {
	//CHECK_ALL: _Array_ptr<_Ptr<struct general>> foo(void) {
        struct general * x = malloc(sizeof(struct general));
	//CHECK: _Ptr<struct general> x = malloc<struct general>(sizeof(struct general));
        struct general * y = malloc(sizeof(struct general));
	//CHECK_NOALL: struct general * y = malloc<struct general>(sizeof(struct general));
	//CHECK_ALL: _Ptr<struct general> y = malloc<struct general>(sizeof(struct general));
        
        struct general *curr = y;
	//CHECK_NOALL: struct general *curr = y;
	//CHECK_ALL: _Ptr<struct general> curr = y;
        int i;
        for(i = 1; i < 5; i++, curr = curr->next) { 
            curr->data = i;
            curr->next = malloc(sizeof(struct general));
            curr->next->data = i+1;
        }
        struct general ** z = sus(x, y);
	//CHECK_NOALL: _Ptr<struct general *> z = sus(x, y);
	//CHECK_ALL: _Array_ptr<_Ptr<struct general>> z = sus(x, y);
return z; }

struct general ** bar() {
	//CHECK_NOALL: struct general **bar(void) : itype(_Ptr<struct general *>) {
	//CHECK_ALL: _Array_ptr<_Ptr<struct general>> bar(void) {
        struct general * x = malloc(sizeof(struct general));
	//CHECK: _Ptr<struct general> x = malloc<struct general>(sizeof(struct general));
        struct general * y = malloc(sizeof(struct general));
	//CHECK_NOALL: struct general * y = malloc<struct general>(sizeof(struct general));
	//CHECK_ALL: _Ptr<struct general> y = malloc<struct general>(sizeof(struct general));
        
        struct general *curr = y;
	//CHECK_NOALL: struct general *curr = y;
	//CHECK_ALL: _Ptr<struct general> curr = y;
        int i;
        for(i = 1; i < 5; i++, curr = curr->next) { 
            curr->data = i;
            curr->next = malloc(sizeof(struct general));
            curr->next->data = i+1;
        }
        struct general ** z = sus(x, y);
	//CHECK_NOALL: struct general ** z = sus(x, y);
	//CHECK_ALL: _Array_ptr<_Ptr<struct general>> z = sus(x, y);
z += 2;
return z; }

struct general ** sus(struct general * x, struct general * y) {
	//CHECK_NOALL: struct general **sus(struct general *x : itype(_Ptr<struct general>), struct general *y : itype(_Ptr<struct general>)) : itype(_Ptr<struct general *>) {
	//CHECK_ALL: _Array_ptr<_Ptr<struct general>> sus(struct general *x : itype(_Ptr<struct general>), _Ptr<struct general> y) {
x = (struct general *) 5; 
	//CHECK: x = (struct general *) 5; 
        struct general **z = calloc(5, sizeof(struct general *));
	//CHECK_NOALL: struct general **z = calloc<struct general *>(5, sizeof(struct general *));
	//CHECK_ALL: _Array_ptr<_Ptr<struct general>> z = calloc<_Ptr<struct general>>(5, sizeof(struct general *));
        struct general *curr = y;
	//CHECK_NOALL: struct general *curr = y;
	//CHECK_ALL: _Ptr<struct general> curr = y;
        int i;
        for(i = 0; i < 5; i++) { 
	//CHECK_NOALL: for(i = 0; i < 5; i++) { 
	//CHECK_ALL: for(i = 0; i < 5; i++) _Checked { 
            z[i] = curr; 
            curr = curr->next; 
        } 
        
z += 2;
return z; }
