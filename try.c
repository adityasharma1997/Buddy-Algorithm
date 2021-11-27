#include <stdio.h>

int log_2(int n) {
  int i = 0;
  while (n >> (i + 1)) i++;
  return i;
}

int main(){
    int x=2000;
    x=x/8;
    int a=log_2(x);
    printf("%d",a);
    return 0;
}