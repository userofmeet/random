#include <stdio.h>

int power2(int, int);
int convert(long);

int main() {
    int i = 0;
    long bin[4];
    int dec[4];

    printf("Enter IP address in binary notation (separate each byte by space):\n");

    for (i = 0; i < 4; i++) {
        scanf("%ld", &bin[i]);
        dec[i] = convert(bin[i]);
    }

    printf("IP address in dotted decimal form:\n");
    printf("%d.%d.%d.%d", dec[0], dec[1], dec[2], dec[3]);

    return 0;
}

int convert(long data) {
    int sum = 0, i;

    for (i = 0; data > 0; i++) {
        sum += (data % 10) * (power2(2, i));
        data = data / 10;
    }

    return sum;
}

int power2(int m, int n) {
    int ans = 1, i;

    for (i = 1; i <= n; i++)
        ans *= m;

    return ans;
}
