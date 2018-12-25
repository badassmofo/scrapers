#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lengthOfLongestSubstring(char* s) {
	int ret = 0, i = 0, j = 0, k = 0;
	size_t length = strlen(s);
	for (i = 0; i < length; ++i) {
		char* tests = (char*)calloc(length, sizeof(char));
		tests[0] = s[i];
		for (j = i + 1, k = 1; j < length; ++j, ++k) {
			char c = s[j];
			if (strchr(tests, c))
				break;
			tests[k] = c;
		}
		free(tests);

		if (k > ret)
			ret = k;
	}
	return ret;
}

int main(void) {
	char* a = "abcabcbb";
	char* b = "bbbbb";
	char* c = "pwwkew";
	char* d = "loddktdji";

	printf("%s = %d\n", a, lengthOfLongestSubstring(a));
	printf("%s = %d\n", b, lengthOfLongestSubstring(b));
	printf("%s = %d\n", c, lengthOfLongestSubstring(c));
	printf("%s = %d\n", d, lengthOfLongestSubstring(d));
}

