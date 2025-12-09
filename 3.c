#include <stdio.h>

int main() {
    int N, M;
    printf("enter N and M number: ");
    scanf("%d %d", &N, &M);

    int a[N][M];
    int i, j;

    // INPUT
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            printf("A[%d][%d] element: ", i, j);
            scanf("%d", &a[i][j]);
        }
    }

    // ROW SUMS
    printf("Row sums:\n");
    for (i = 0; i < N; i++) {
        int sum = 0;
        for (j = 0; j < M; j++)
            sum += a[i][j];
        printf("%d ", sum);
    }
    printf("\n");

    // COLUMN SUMS
    printf("Column sums:\n");
    for (j = 0; j < M; j++) {
        int sum = 0;
        for (i = 0; i < N; i++)
            sum += a[i][j];
        printf("%d ", sum);
    }
    printf("\n");

    return 0;
}
