#include<stdio.h>
#include<windows.h>
int main(){
  HANDLE hout=GetStdHandle(STD_OUTPUT_HANDLE);
  for(int i=1;i<255;i++){
    SetConsoleTextAttribute(hout,i);
    printf("%3d\n",i);
  }
  SetConsoleTextAttribute(hout,7);
  return 0;
}
