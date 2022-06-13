#include <stdio.h>
#include <stdlib.h>
#include "selectors.h"

int progress = 0;

/* If RANDOM_SELS = YES, then it progresses through selectors in a random order
   and never halts. If it's NO, then it treats the selector as a binary integer
   and methodically tries each, eventually halting after evaluating all
   possible selectors for this set of parameters.
   Useful for if you want a broad survey of selectors for large n
*/
#define RANDOM_SELS NO

#define REDUCIBLE NO

int main(int argc, char *argv[]) {

    if(argc < 4 || argc > 6) {
        printf("usage: sel [n] [k] [r] (u) (i)\n");
        printf("       n- alphabet size\n");
        printf("       k- subset size\n");
        printf("       r- minimum number of selected elements\n");
        printf("       u- upper limit on size of selectors displayed (may be "
                "more interesting to see selectors of size less than n)\n");
        printf("       i- 'i' if you want to only display \"interesting\" "
                "selectors (those with some set containing more than one "
                "element\n");
        return -1;
    }

    int N, K, R, upper_len;


    // 'i' if you only want to print interesting selectors (have some set with
    // more than one element), 'n' otherwise
    char interest = 'n';

    if(argc >= 4) {
        N = atoi(argv[1]);
        upper_len = N;
        K = atoi(argv[2]);
        R = atoi(argv[3]);
        if(argc == 5) {
            if('0' <= argv[4][0] && argv[4][0] <= '9') {
                upper_len = atoi(argv[4]);
            } else if('A' <= argv[4][0] && argv[4][0] <= 'z') {
                interest = argv[4][0];
            }
        }
        if(argc == 6) {
            upper_len = atoi(argv[4]);
            interest = argv[5][0];
        }
    }

    if(R <= 0 || R > K || K > N) {
        printf("Invalid n, k, r parameters\n");
        return -1;
    }

    if(upper_len > N || upper_len <= 0) {
        printf("Be aware you set the upper limit on size of selectors "
                "displayed to %d\n", upper_len);
    }


    printf("Starting\n");	

    sel temp_sel;
    incr_sel(&temp_sel, YES, N);
    int ret;

    int ct = 0;
    while(1) {

        if(REDUCIBLE == NO && temp_sel.len <= upper_len && 
                is_sel(&temp_sel, N, K, R) == YES) {
            print_sel(&temp_sel, interest, N, K, R);
            ++ct;
        } else if(REDUCIBLE == YES && temp_sel.len <= upper_len) {
            int i, is_red = YES;
            for(i = N; i > 0; --i) {
                int r = ceiling((((double) R) / K) * i);
                if(is_sel(&temp_sel, N, i, r) == NO) is_red = NO;
            }

            if(is_red == YES) {
                print_sel(&temp_sel, interest, N, K, R);
                ++ct;
            }
           //***************************************************** 
        }

        if(incr_sel(&temp_sel, NO, N) == -1) break;
    }

    printf("Completed! %d specified selectors found", ct);
    if(interest == 'i') printf(" (not all may have been printed due to "
            "interest setting)");
    printf("\n");
	return 0;
}

int ceiling(double d) {
    if(d - (int) d < EPSILON) return (int) d;
    return (int) (d + 1);
}

// "Increments" the selector like a binary integer
int incr_sel(sel *old_sel, int first_time, int N) {

    // Only takes this path exactly once, while examining the first selector
    if(first_time == YES) {

        // Only reaches this malloc once
        old_sel->family = (int**) malloc(sizeof(int*) * N);
        int i;
        for(i = 0; i < N; ++i) {
            old_sel->family[i] = (int*) malloc(sizeof(int) * N);
        }

        for(i = 0; i < N * N; ++i) old_sel->family[i/N][i%N] = NO;

        old_sel->len = 0;
        return 0;
    }

    if(RANDOM_SELS == YES) {
        old_sel->len = 0;
        int i;
        int len_incr_already;
        int ran;
        for(i = 0; i < N*N; ++i) {

            if(i % N == 0) len_incr_already = NO;

            ran = rand();
            if(ran % 2 == 0) old_sel->family[i/N][i%N] = NO;
            else {
                old_sel->family[i/N][i%N] = YES;
                if(len_incr_already == NO) {
                    old_sel->len += 1;
                    len_incr_already = YES;
                }
            }
        }
        return 0;
    }

    int ind = N*N - 1;
    int ret = incr_sel_recurs(old_sel, ind, N);
    old_sel->len = count_sets(old_sel, N);
    return ret;
}

int incr_sel_recurs(sel *old_sel, int ind, int N) {
    if(old_sel->family[ind/N][ind%N] == NO) {
        old_sel->family[ind/N][ind%N] = YES;

        // Progress output
        if(ind == 4) {
            printf("\t\t\t\t\t\tProgress: %d%%\n", (int)(6.25*progress));
            progress++;
        }

    } else {
        if(ind == 0) return -1;
        old_sel->family[ind/N][ind%N] = NO;
        return incr_sel_recurs(old_sel, ind - 1, N);
    }

    return 0;
}

// "Increments" the testing array
int next_arr(int *k_arr, int *level, int N, int K) {

    int i;
    // Initialization
    if(k_arr[0] == -1) {
        for(i = 0; i < K; ++i) k_arr[i] = i + 1;
        return 0;
    } 

    // Increment the rightmost element that's possible to increment and reset
    // those to its right if there's overflow
    for(i = K-1; i >= 0; --i) {
        if(k_arr[i] < N && (i == K-1 || k_arr[i] < k_arr[i + 1] - 1)) {

            k_arr[i]++;

            int j;
            for(j = i + 1; j <= K-1; ++j) k_arr[j] = k_arr[i] + (j - i);

            return 0;
        }
    }

    return -1;
}

// Returns whether this is an (N,K,R)-selector
int is_sel(sel *in, int N, int K, int R) {
    int k_arr[K]; // Array of the elements in the K-subset of [N]
    int i, j, level = 0;
    for(i = 0; i < K; ++i) k_arr[i] = -1;

    while(1) {

        int ret = next_arr(k_arr, &level, N, K);
        if(ret == -1) break;

        int num_selects = 0;
        int selections[N];
        int selected_elem;
        for(i = 0; i < N; ++i) selections[i] = NO;

        for(j = 0; j < N; ++j) {

            
            // If they intersect in exactly one element,
            if((selected_elem = intersect(in->family[j], k_arr, N, K)) != -1) {
                // If this element hasn't been selected before, increment
                if(selections[selected_elem - 1] == NO) {
                    num_selects++;
                }
                selections[selected_elem - 1] = YES;
            }
        }
        if(num_selects < R) {
            return NO;
        }
    }

    return YES;
}

/* Returns -1 if intersection size is not exactly 1, returns the selected
   element otherwise
   a is an array of N integers, b is an array of K integers
*/
int intersect(int *a, int *b, int N, int K) {
    int intersection = -1;
    int i, j;
    for(i = 0; i < K; ++i) {
        if(a[b[i] - 1] == YES) {
            /*printf("[");
            for(j = 0; j < N; ++j) printf("%d,", a[j]);
            printf(" overlap %d]", b[i]);*/
            if(intersection != -1) {
                //printf(" {>1 intersection} ");
                return -1;
            }
            intersection = b[i];
        }
    }
    //if(intersection == -1) printf(" {0 intersections} ");
    return intersection;
}

// Prints a representation of the passed selector
void print_sel(sel *p, char onlyPrintInteresting, int N, int K, int R) {
    int interesting = NO;
    int set, elem;

    if(onlyPrintInteresting == 'i') {
        for(set = 0; set < N; ++set) {
            int numElems = 0;
            for(elem = 0; elem < N; ++elem) {
                if(p->family[set][elem] == YES) {
                    ++numElems;
                }
                if(numElems > 1) interesting = YES;
            }
        }
        if(interesting == NO) return;
    }

    if(REDUCIBLE == YES) printf("Reducible ");
    printf("(%d, %d, %d)-selector of length %d\n", N, K, R, p->len);
    for(set = 0; set < N; ++set) {
        int no_cntr = 0;
        printf("[ ");
        for(elem = 0; elem < N; ++elem) {
            if(p->family[set][elem] == YES) {
                printf("%d ", elem + 1);
            } else no_cntr++;
        }
        printf("]%*s", 3 + no_cntr*2, "[ ");
        for(elem = 0; elem < N; ++elem) {
            if(p->family[set][elem] == YES) {
                printf("1 ");
            } else printf("0 ");
        }
        printf("]\n");
    }
    printf("\n\n");
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

// Count the number of nonempty sets in the passed selector
int count_sets(sel *in, int N) {
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
