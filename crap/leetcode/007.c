#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int reverse(int x) {
	long long ret = 0;
	while (x) {
		ret = ret * 10 + x % 10;
		x /= 10;
	}
	return (ret < INT_MIN || ret > INT_MAX) ? 0 : ret;
}

int main(void) {
	printf("%d = %d\n", 123, reverse(123));
	printf("%d = %d\n", -123, reverse(-123));
	return 0;
}
