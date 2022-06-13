
#define YES 1
#define NO  0
#define MASK 0x0000000000000001

#define EPSILON 0.00001

typedef struct selector { 
    int **family;
    int len; 
} sel;

void print_sel(sel *p, char printOnlyInteresting, int N, int K, int R);
int count_sets(sel *in, int N);
int incr_sel_recurs(sel *old_sel, int ind, int N);
int incr_sel(sel *old_sel, int first_time, int N);
int my_log(unsigned int num);
int intersect(int *a, int *b, int N, int K);
int next_arr(int *k_arr, int *level, int N, int K);
int is_sel(sel *in, int N, int K, int R);
int ceiling(double d);
