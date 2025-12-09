#include <stdio.h>

struct EngiinButarxai {
    int d, m; 
};
typedef struct EngiinButarxai EngiinButarxai;

EngiinButarxai add(EngiinButarxai a, EngiinButarxai b) {
    EngiinButarxai result;
    result.d = a.d * b.m + b.d * a.m;
    result.m = a.m * b.m;
    return result;
}


EngiinButarxai sub(EngiinButarxai a, EngiinButarxai b) {
    EngiinButarxai result;
    result.d = a.d * b.m - b.d * a.m;
    result.m = a.m * b.m;
    return result;
}

EngiinButarxai mult(EngiinButarxai a, EngiinButarxai b) {
    EngiinButarxai result;
    result.d = a.d * b.d;
    result.m = a.m * b.m;
    return result;
}

EngiinButarxai div(EngiinButarxai a, EngiinButarxai b) {
    EngiinButarxai result;
    result.d = a.d * b.m;
    result.m = a.m * b.d;
    return result;
}

void print(EngiinButarxai a) {
    printf("%d/%d\n", a.d, a.m);
}

int main() {
    EngiinButarxai a = {3, 4};  
    EngiinButarxai b = {2, 5};
    
    printf("a = ");
    print(a);
    printf("b = ");
    print(b);
    
    printf("\nAddition: ");
    print(add(a, b));
    
    printf("Subtraction: ");
    print(sub(a, b));
    
    printf("Multiplication: ");
    print(mult(a, b));
    
    printf("Division: ");
    print(div(a, b));
    printf("\n");
    return 0;
}