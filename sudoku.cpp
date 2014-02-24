/*
 * Sudoku Solver
 * Rohit Girdhar
 * Ref: http://lists.gnu.org/archive/html/help-glpk/2006-03/msg00013.html
 */

#include <cstdio>
#include <cstdlib>
#include <glpk.h>
#include <vector>
#include <string>
#define N 9
#define RN 3
using namespace std;


string getName(int i, int j, int k) {
    string ans = "x";
    ans += char('1'+i);
    ans += (char)'1'+j;
    ans += (char)'1'+k;
//    printf("%s\n", ans.c_str());
    return ans;
}

void sudokuSolver(int prob[][N]) {
    glp_prob *lp;
    int ia[100000], ja[100000];
    double ar[100000], z;
    lp = glp_create_prob();
    glp_set_prob_name(lp,"SudokuSolver");
    glp_set_obj_dir(lp,GLP_MAX);

    // x[i,j,k] = 1; if i,j position 
    int i,j,k,r,c;
    int rno = 1;

    //count number of knowns
    int cnt = 0;
    for(i=0; i<N; ++i) for(j=0; j<N; ++j) if(prob[i][j] > 0) cnt++;

    glp_add_rows(lp, cnt+4*N*N);
    //assign predef nos.
    for(r=0; r<cnt; ++r) {
        glp_set_row_bnds(lp,rno++,GLP_FX, 1, 1);
    }
    // each i,j can have exactly one elt
    for(i=0; i<N*N; ++i) {
        glp_set_row_bnds(lp, rno++, GLP_FX, 1,1);
    }

    //cells in same row, col,rgn must have different numbers
    for(i=0; i<3*N*N; ++i) {
        glp_set_row_bnds(lp, rno++, GLP_FX, 1,1);
    }

    
    glp_add_cols(lp, N*N*N);
    int cno = 1;
    for(i=0; i<N; i++) {
        for(j=0; j<N; j++) {
            for(k=0; k<N; k++) {
                string nm = getName(i,j,k);
                glp_set_col_name(lp, cno, nm.c_str());
                glp_set_col_kind(lp, cno, GLP_BV);
            //    glp_set_col_stat(lp, cno, GLP_BS);
                glp_set_col_bnds(lp, cno, GLP_DB, 0, 1);
                glp_set_obj_coef(lp, cno, 0);
                cno++;
            }
        }
    }

    int p = 1;
    rno = 1;
    for(r=0; r<N; ++r) {
        for(c=0; c<N; ++c) {
            if(prob[r][c] > 0) {
                ia[p] = rno++; ja[p] = r*N*N + c*N + (prob[r][c]); ar[p] = 1.0;
                p++;
            }
        }
    }
    
    for(i=0; i<N; ++i) {
        for(j=0; j<N; ++j) {
            for(k=0; k<N; ++k) {
                ia[p] = rno; ja[p] = i*N*N + j*N + k + 1; ar[p] = 1.0;
                p++;
            }
            rno++;
        }
    }

    for(i=0; i<N; ++i) {
        for(k=0; k<N; ++k) {
            for(j=0; j<N; ++j) {
                ia[p] = rno; ja[p] = i*N*N + j*N + k + 1; ar[p] = 1.0;
                p++;
            }
            rno++;
        }
    }
    for(j=0; j<N; ++j) {
        for(k=0; k<N; ++k) {
            for(i=0; i<N; ++i) {
                ia[p] = rno; ja[p] = i*N*N + j*N + k + 1; ar[p] = 1.0;
                p++;
            }
            rno++;
        }

    }
    int I,J;
    for(I=0; I<N; I+=RN) {
        for(J=0; J<N; J+=RN) {
            for(k=0; k<N; ++k) {
                for(i=I; i<I+RN; i++) {
                    for(j=J; j<J+RN; j++) {
                        ia[p] = rno; ja[p] = i*N*N + j*N + k + 1; ar[p] = 1.0;
                        p++;
                    }
                }
                rno++;
            }
        }
    }
    glp_load_matrix(lp, p-1, ia, ja, ar);
    glp_iocp param;
    glp_init_iocp(&param);
    param.presolve = GLP_ON;
    glp_intopt(lp, &param);
    //glp_simplex(lp,NULL);
    for(i=0; i<N; ++i) {
        for(j=0; j<N; ++j) {
            for(k=0; k<N; ++k) {
                int temp = glp_mip_col_val(lp, i*N*N + j*N + k + 1);
                if(temp != 0) {
                    prob[i][j] = k+1;
                }
            }
        }
    }
    glp_delete_prob(lp);
}

int main() {
    int a[9][9] = {{6,0,0,1,0,8,2,0,3},
                   {0,2,0,0,4,0,0,9,0},
                   {8,0,3,0,0,5,4,0,0},
                   {5,0,4,6,0,7,0,0,9},
                   {0,3,0,0,0,0,0,5,0},
                   {7,0,0,8,0,3,1,0,2},
                   {0,0,1,7,0,0,9,0,6},
                   {0,8,0,0,3,0,0,2,0},
                   {3,0,2,9,0,4,0,0,5}};
    int b[9][9] = {{0,0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0,0},
                   {0,0,0,0,0,0,0,0,0}};

    sudokuSolver(b);
    int i,j;
    for(i=0; i<N; ++i) {
        for(j=0; j<N; ++j) {
            printf("%d ", b[i][j]);
        }
        printf("\n");
    }
    return 0;

}
