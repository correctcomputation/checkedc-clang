// RUN: rm -rf %t*
// RUN: 3c -base-dir=%S -alltypes -addcr %s -- | FileCheck -match-full-lines -check-prefixes="CHECK_ALL","CHECK" %s
// RUN: 3c -base-dir=%S -addcr %s -- | FileCheck -match-full-lines -check-prefixes="CHECK_NOALL","CHECK" %s
// RUN: 3c -base-dir=%S -addcr %s -- | %clang -c -fcheckedc-extension -x c -o /dev/null -
// RUN: 3c -base-dir=%S -output-dir=%t.checked -alltypes %s --
// RUN: 3c -base-dir=%t.checked -alltypes %t.checked/valist.c -- | diff %t.checked/valist.c -

#include <stdarg.h>
typedef int lua_State;
extern void lua_lock(lua_State *);
extern void luaC_checkGC(lua_State *);
extern void lua_unlock(lua_State *);
extern const char *luaO_pushvfstring (lua_State *L, const char *fmt, va_list argp);
const char *lua_pushfstring (lua_State *L, const char *fmt, ...) {
	//CHECK: const char *lua_pushfstring(lua_State *L : itype(_Ptr<lua_State>), const char *fmt : itype(_Ptr<const char>), ...) : itype(_Ptr<const char>) {
  const char *ret;
	//CHECK: const char *ret;
  va_list argp;
  lua_lock(L);
  va_start(argp, fmt);
  ret = luaO_pushvfstring(L, fmt, argp);
  va_end(argp);
  luaC_checkGC(L);
  lua_unlock(L);
  return ret;
}
/*force output*/
int *p;
	//CHECK: _Ptr<int> p = ((void *)0);

// This tests va_list correctness using windows builtins
void foo( const char *fmt, __builtin_ms_va_list argp);
__attribute__((ms_abi)) int *bar(const char *fmt, ...)  {
//CHECK: __attribute__((ms_abi)) _Ptr<int> bar(const char *fmt : itype(_Ptr<const char>), ...)  {
  __builtin_ms_va_list argp;
  __builtin_ms_va_start(argp, fmt);
  //CHECK: __builtin_ms_va_start(argp, fmt);
  foo(fmt, argp);
  __builtin_ms_va_end(argp);
  //CHECK: __builtin_ms_va_end(argp);
  return 0;
}
