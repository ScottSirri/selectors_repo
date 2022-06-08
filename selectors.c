#include <stdio.h>
#include <stdlib.h>

/*int N = 4;
int K = 3;
int R = 3;*/

#define N 5
#define K 3
#define R 2

#define YES 1
#define NO  0
#define MASK 0x0000000000000001

int upper_len = N - 1;
int progress = 0;

typedef struct selector { 
    int family[N][N];
    int len; 
} sel;

// Prints a representation of the passed selector
void print_sel(sel *p) {
    int set, elem;
    printf("%d\n", p->len);
    for(set = 0; set < N; ++set) {
        printf("[ ");
        for(elem = 0; elem < N; ++elem) {
            if(p->family[set][elem] == YES) printf("%d ", elem + 1);
        }
        printf(" ]\n");
    }
}

int count_sets(sel *in) {
    int ct = 0;
    int i, j;
    for(i = 0; i < N; ++i) {
        int empty = YES;
        for(j = 0; j < N; ++j) {
            if(in->family[i][j] == YES) empty = NO;
        }
        if(empty == NO) ++ct;
    }
    return ct;
}

int incr_sel_recurs(sel *old_sel, int ind) {
    if(old_sel->family[ind/N][ind%N] == NO) {
        old_sel->family[ind/N][ind%N] = YES;
        if(ind == 4) {
            printf("Progress: %d%%\n", (int)(6.25*progress));
            progress++;
        }
    } else {
        if(ind == 0) return -1;
        old_sel->family[ind/N][ind%N] = NO;
        return incr_sel_recurs(old_sel, ind - 1);
    }

    return 0;
}

// "Increments" the selector like a binary integer
int incr_sel(sel *old_sel, int first_time) {
    if(first_time == YES) {
        int i;
        for(i = 0; i < N * N; ++i) old_sel->family[i/N][i%N] = NO;
        old_sel->len = 0;
        return 0;
    }

    int ind = N*N - 1;
    int ret = incr_sel_recurs(old_sel, N*N - 1);
    old_sel->len = count_sets(old_sel);
    return ret;
}


// Returns log base 2 rounded up
int my_log(unsigned int num) {
    int l = 0;
    while(num > 0) {
        ++l;
        num = num>>1;
    }
    return l;
}

// Returns -1 if intersection size is not exactly 1, returns the selected
// element otherwise
int intersect(int a[N], int b[K]) {
    int intersection = -1;
    int i;
    for(i = 0; i < K; ++i) {
        if(a[b[i] - 1] == YES) {
            if(intersection != -1) return -1;
            intersection = b[i];
        }
    }
    return intersection;
}

// "Increments" the testing array
int next_arr(int k_arr[K], int *level) {
    int i;
    if(k_arr[0] == -1) {
        for(i = 0; i < K; ++i) k_arr[i] = i + 1;
        //printf("\tdefault arr\n");
        return 0;
    } 

    // Increment the rightmost element that's possible to increment
    // Fails if all K elements are all the way, consecutive, to the right
    for(i = K-1; i >= 0; --i) {
        if(k_arr[i] < N && (i == K-1 || k_arr[i] < k_arr[i + 1] - 1)) {
            k_arr[i]++;
            //printf("\tincremented arr\n");
            return 0;
        }
    }

    // Reset to the next cluster of K consecutive but starting one element 
    // higher than last iteration
    (*level) += 1;

    if(*level >= N - K) {
        return -1;
    }

    for(i = *level; i - *level < K; ++i) {
        //printf("\treset arr\n");
        k_arr[i - *level] = i;
    }

    return 0;
}

// Returns whether this is an (N,K,R)-selector
int is_sel(sel *in) {
    //printf("testing sel\n");
    int k_arr[K]; // Array of the elements in the K-subset of [N]
    int i, j, level = 0;
    for(i = 0; i < K; ++i) k_arr[i] = -1;

    while(1) {
        int ret = next_arr(k_arr, &level);
        if(ret == -1) break;

        int num_selects = 0;
        int selections[N];
        int selected_elem;
        for(i = 0; i < N; ++i) selections[i] = NO;

        for(j = 0; j < N; ++j) {
            // If they intersect in at exactly one element,
            if((selected_elem = intersect(in->family[j], k_arr)) != -1) {
                // If this element hasn't been selected before, increment
                if(selections[selected_elem - 1] == NO) num_selects++;
                selections[selected_elem - 1] = YES;
            }
        }
        if(num_selects < R) {
            return NO;
        }
    }

    return YES;
}

int main(int argc, char *argv[]) {

    /*if(argc >= 4) {
        N = atoi(argv[1]);
        K = atoi(argv[2]);
        R = atoi(argv[3]);
    }*/

    if(R <= 0 || R > K || K > N) {
        printf("Invalid n, k, r parameters\n");
        return -1;
    }

    if(argc == 2) upper_len = atoi(argv[1]);

    printf("Starting\n");	

    sel temp_sel;
    incr_sel(&temp_sel, YES);

    int ct = 0;
    while(1) {
        if(temp_sel.len <= upper_len && is_sel(&temp_sel) == YES) {
            print_sel(&temp_sel);
            printf("\n\n");
            ++ct;
        }
        /*print_sel(&temp_sel);
        printf("\n\n00000000000000000 NEXT 0000000000000\n\n");*/
        if(incr_sel(&temp_sel, NO) == -1) break;
    }

    printf("Last considered: \n");
    print_sel(&temp_sel);

    printf("Completed! %d\n", ct);
	return 0;
}
