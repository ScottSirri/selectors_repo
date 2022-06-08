
int N = 6;
int K = 3;
int R = 2;

#define YES 1
#define NO  0
#define MASK 0x0000000000000001

typedef struct selector { 
    int **family;
    int len; 
} sel;

void print_sel(sel *p, char printOnlyInteresting);
int count_sets(sel *in);
int incr_sel_recurs(sel *old_sel, int ind);
int incr_sel(sel *old_sel, int first_time);
int my_log(unsigned int num);
int intersect(int a[N], int b[K]);
int next_arr(int k_arr[K], int *level);
int is_sel(sel *in);
