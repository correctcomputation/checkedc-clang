// RUN: rm -rf %t*
// RUN: 3c -base-dir=%S -addcr %s -- | FileCheck -match-full-lines --check-prefixes="CHECK" %s
// RUN: 3c -base-dir=%S -addcr %s -- | %clang -c -fcheckedc-extension -x c -o %t.unused -

int foo(int x) { 
  //CHECK: int foo(int x) {
  x += non();
  return x;
}


// Dummy to cause output
int dummy(int *x) { 
  return *x;
}
