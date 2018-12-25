#include <stdio.h>
#include <stdlib.h>

int test[4] = { 2, 7, 11, 15 };
int target  = 9;

int* twoSum(int* nums, int numsSize, int target) {
	for (int i = 0; i < numsSize; ++i) {
		for (int j = 0; j < numsSize; ++j) {
			if (i == j)
				continue;

			if (nums[i] + nums[j] == target) {
				int* ret = (int*)calloc(2, sizeof(int));
				ret[0] = i;
				ret[1] = j;
				return ret;
			}
		}
	}
	return NULL;
}

int main(void) {
	int* result = twoSum(&test, 4, target);
	if (!result)
		return -1;

	printf("%d + %d = %d\n", result[0], result[1], target);
	free(result);
	return 0;
}
