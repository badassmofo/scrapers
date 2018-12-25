#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define stb_sb_free(a)         ((a) ? free(stb__sbraw(a)),0 : 0)
#define stb_sb_push(a,v)       (stb__sbmaybegrow(a,1), (a)[stb__sbn(a)++] = (v))
#define stb_sb_count(a)        ((a) ? stb__sbn(a) : 0)
#define stb_sb_add(a,n)        (stb__sbmaybegrow(a,n), stb__sbn(a)+=(n), &(a)[stb__sbn(a)-(n)])
#define stb_sb_last(a)         ((a)[stb__sbn(a)-1])

#define stb__sbraw(a) ((int *) (a) - 2)
#define stb__sbm(a)   stb__sbraw(a)[0]
#define stb__sbn(a)   stb__sbraw(a)[1]

#define stb__sbneedgrow(a,n)  ((a)==0 || stb__sbn(a)+(n) >= stb__sbm(a))
#define stb__sbmaybegrow(a,n) (stb__sbneedgrow(a,(n)) ? stb__sbgrow(a,n) : 0)
#define stb__sbgrow(a,n)      ((a) = stb__sbgrowf((a), (n), sizeof(*(a))))

static void * stb__sbgrowf(void *arr, int increment, int itemsize) {
	int dbl_cur = arr ? 2*stb__sbm(arr) : 0;
	int min_needed = stb_sb_count(arr) + increment;
	int m = dbl_cur > min_needed ? dbl_cur : min_needed;
	int *p = (int *) realloc(arr ? stb__sbraw(arr) : 0, itemsize * m + sizeof(int)*2);
	if (p) {
		if (!arr)
			p[1] = 0;
		p[0] = m;
		return p+2;
	} else {
#ifdef STRETCHY_BUFFER_OUT_OF_MEMORY
		STRETCHY_BUFFER_OUT_OF_MEMORY ;
#endif
		return (void *) (2*sizeof(int)); // try to force a NULL pointer exception later
	}
}

typedef struct {
	char* path;
	unsigned int dist;
} file_t;

int comp(const void* _a, const void* _b) {
	file_t *a = (file_t*)_a, *b = (file_t*)_b;
	return b->dist - a->dist;
}

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int dist(const char* a, const char* b) {
	unsigned int a_len = strlen(a),
							 b_len = strlen(b),
							 y, l, o,
							 c[a_len + 1];
	for (y = 1; y <= a_len; y++)
		c[y] = y;
	for (unsigned int x = 1; x <= b_len; ++x) {
		c[0] = x;
		for (y = 1, l = x - 1; y <= a_len; ++y) {
			o = c[y];
			c[y] = MIN3(c[y] + 1, c[y - 1] + 1, l + (a[y - 1] == b[x - 1] ? 0 : 1));
			l = o;
		}
	}
	return c[a_len];
}

int walk(const char* p, const char* fz) {
	char fn[FILENAME_MAX];
	int p_len = strlen(p);
	if (p_len >= FILENAME_MAX - 1) {
		fprintf(stderr, "Filename too long: \"%s\"", p);
		return 1;
	}
	strcpy(fn, p);
	fn[p_len++] = '/';

	DIR* d;
	if (!(d = opendir(p))) {
		fprintf(stderr, "Can't open: \"%s\"", p);
		return 1;
	}

	struct stat st;
	struct dirent* e;
	while ((e = readdir(d))) {
		if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, ".."))
			continue;

		strncpy(fn + p_len, e->d_name, FILENAME_MAX - p_len);
		if (lstat(fn, &st) == -1) {
			fprintf(stderr, "Can't stat \"%s\"", fn);
			return 1;
		}

		// if (S_ISLNK(st.st_mode))
		//	continue

		if (S_ISDIR(st.st_mode))
			if (walk(fn, fz))
				return 1;

		printf("%s: %d\n", fn, dist(fn, fz));
	}

	if (d)
		closedir(d);
	return 0;
}

int main(int argc, char* argv[]) {
	return walk(".", argv[1]);
}
