#include <stdio.h>

int main() {

  int i = 0, n;
  printf("enter  n number: ");
  scanf("%d", &n);
  int A[8] = {0};
  while ( n > 0 || i < 8) {
    A[i] = n % 2;
    n /= 2;
    i++;
  }

  for(i = 7; i >= 0; i--) {
    printf("%d", A[i]);
  }
  printf("\n");
}