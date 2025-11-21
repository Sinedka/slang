#include <library>
#include "file.sl"

// comment
// <type>: byte | short | int | long

//normal func
<type> funcName(<type> parametr) {
  return parametr + 3
}

void callF(){
  
}

//opearations with fuctions()
//int Test(int in) {
//  print(in)
//  return 0;
//}
//
//func type: 
// Test(int)->int
//pointer:
// Test(int)->int*


// operators
// unar operators:
// + - ++ -- ~ !
// binary operators:
// + - / * % ^ | & 
// additional opearators
// a ? b : c
// a[b]
// (  )

int main() {
  print(funcName(5))
}
