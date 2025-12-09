 #include <stdio.h>
 typedef struct {
  int a[2];
  double d;
 } struct_t;
 double fun(int i) {
  volatile struct_t s;
  if( i > 2 || i < 0) {
    printf("index hetersen");
    return 0;
  }
  s.d = 3.14;
  s.a[i] = 1073741824;
  return s.d;
 }

int main() {
  int n;
  printf("enter n number: ");
  scanf("%d", &n);
  printf("\n");
  return 0;
}