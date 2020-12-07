// RUN: 3c -base-dir=%S -addcr -alltypes -output-postfix=checkedALL %s %S/fptrunsafecalleemulti2.c
// RUN: 3c -base-dir=%S -addcr -output-postfix=checkedNOALL %s %S/fptrunsafecalleemulti2.c
// RUN: %clang -c %S/fptrunsafecalleemulti1.checkedNOALL.c %S/fptrunsafecalleemulti2.checkedNOALL.c
// RUN: FileCheck -match-full-lines -check-prefixes="CHECK_NOALL","CHECK" --input-file %S/fptrunsafecalleemulti1.checkedNOALL.c %s
// RUN: FileCheck -match-full-lines -check-prefixes="CHECK_ALL","CHECK" --input-file %S/fptrunsafecalleemulti1.checkedALL.c %s
// RUN: 3c -base-dir=%S -alltypes -output-postfix=checked %S/fptrunsafecalleemulti2.c %s
// RUN: 3c -base-dir=%S -alltypes -output-postfix=convert_again %S/fptrunsafecalleemulti1.checked.c %S/fptrunsafecalleemulti2.checked.c
// RUN: test ! -f %S/fptrunsafecalleemulti1.checked.convert_again.c
// RUN: test ! -f %S/fptrunsafecalleemulti2.checked.convert_again.c
// RUN: rm %S/fptrunsafecalleemulti1.checkedALL.c %S/fptrunsafecalleemulti2.checkedALL.c
// RUN: rm %S/fptrunsafecalleemulti1.checkedNOALL.c %S/fptrunsafecalleemulti2.checkedNOALL.c
// RUN: rm %S/fptrunsafecalleemulti1.checked.c %S/fptrunsafecalleemulti2.checked.c


/*********************************************************************************/

/*This file tests three functions: two callers bar and foo, and a callee sus*/
/*In particular, this file tests: converting the callee into a function pointer
unsafely via cast and using that pointer for computations*/
/*For robustness, this test is identical to fptrunsafeprotocallee.c and fptrunsafecallee.c except in that
the callee and callers are split amongst two files to see how
the tool performs conversions*/
/*In this test, foo and bar will treat their return values safely, but sus will
not, through invalid pointer arithmetic, an unsafe cast, etc*/

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
	//CHECK: struct general *next;
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

static int add1(int x) { 
	//CHECK: static int add1(int x) _Checked { 
    return x+1;
} 

static int sub1(int x) { 
	//CHECK: static int sub1(int x) _Checked { 
    return x-1; 
} 

static int fact(int n) { 
	//CHECK: static int fact(int n) _Checked { 
    if(n==0) { 
        return 1;
    } 
    return n*fact(n-1);
} 

static int fib(int n) { 
	//CHECK: static int fib(int n) _Checked { 
    if(n==0) { return 0; } 
    if(n==1) { return 1; } 
    return fib(n-1) + fib(n-2);
} 

static int zerohuh(int n) { 
	//CHECK: static int zerohuh(int n) _Checked { 
    return !n;
}

static int *mul2(int *x) { 
	//CHECK: static _Ptr<int> mul2(_Ptr<int> x) _Checked { 
    *x *= 2; 
    return x;
}

int * sus(struct general *, struct general *);
	//CHECK: int * sus(struct general *, struct general *);

int * foo() {
	//CHECK: int *foo(void) : itype(_Ptr<int>) {

        struct general *x = malloc(sizeof(struct general)); 
	//CHECK: struct general *x = malloc<struct general>(sizeof(struct general)); 
        struct general *y = malloc(sizeof(struct general));
	//CHECK: struct general *y = malloc<struct general>(sizeof(struct general));
        struct general *curr = y;
	//CHECK: struct general *curr = y;
        int i;
        for(i = 1; i < 5; i++, curr = curr->next) { 
            curr->data = i;
            curr->next = malloc(sizeof(struct general));
            curr->next->data = i+1;
        }
        int (*sus_ptr)(struct fptr *, struct fptr *) = sus;   
	//CHECK: int (*sus_ptr)(struct fptr *, struct fptr *) = sus;   
        int *z = (int *) sus_ptr(x, y);
	//CHECK: int *z = (int *) sus_ptr(x, y);
        
return z; }

int * bar() {
	//CHECK: int *bar(void) : itype(_Ptr<int>) {

        struct general *x = malloc(sizeof(struct general)); 
	//CHECK: struct general *x = malloc<struct general>(sizeof(struct general)); 
        struct general *y = malloc(sizeof(struct general));
	//CHECK: struct general *y = malloc<struct general>(sizeof(struct general));
        struct general *curr = y;
	//CHECK: struct general *curr = y;
        int i;
        for(i = 1; i < 5; i++, curr = curr->next) { 
            curr->data = i;
            curr->next = malloc(sizeof(struct general));
            curr->next->data = i+1;
        }
        int (*sus_ptr)(struct fptr *, struct fptr *) = sus;   
	//CHECK: int (*sus_ptr)(struct fptr *, struct fptr *) = sus;   
        int *z = (int *) sus_ptr(x, y);
	//CHECK: int *z = (int *) sus_ptr(x, y);
        
return z; }
