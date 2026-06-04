/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

// @Adan //

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k;
// 极其生硬地定义 8 个辅助变量，逼迫编译器将其放入寄存器
    int v1, v2, v3, v4, v5, v6, v7, v8; 

    for (i = 0; i < 64; i += 8) {
        for (j = 0; j < 64; j += 8) {
            
            // ==================== 前半部分 ====================
            // 处理 A 的上半部分（4行），同时填充 B 的左上和右上
            for (k = 0; k < 4; k++) {
                // 一口气读出 A 当前行的 8 个数
                v1 = A[i + k][j + 0]; v2 = A[i + k][j + 1]; v3 = A[i + k][j + 2]; v4 = A[i + k][j + 3];
                v5 = A[i + k][j + 4]; v6 = A[i + k][j + 5]; v7 = A[i + k][j + 6]; v8 = A[i + k][j + 7];

                // 转置写入 B 的左上角
                B[j + 0][i + k] = v1; B[j + 1][i + k] = v2; B[j + 2][i + k] = v3; B[j + 3][i + k] = v4;
                
                // 顺手写入 B 的右上角（注意：此时这里并没有完成转置，只是临时存放！）
                B[j + 0][i + k + 4] = v5; B[j + 1][i + k + 4] = v6; B[j + 2][i + k + 4] = v7; B[j + 3][i + k + 4] = v8;
            }

            // ==================== 后半部分（魔鬼细节） ====================
            // 此时，B 的右上角存着原本属于 B 左下角的数据。
            // 我们需要按列遍历：一方面读出 B 右上角的数据，一方面读出 A 左下角的数据，
            // 然后完美的把它们安置到正确的位置。
            for (k = 0; k < 4; k++) {
                // 1. 读出 A 左下角的一列（4个数）
                v1 = A[i + 4][j + k]; v2 = A[i + 5][j + k]; v3 = A[i + 6][j + k]; v4 = A[i + 7][j + k];
                
                // 2. 读出 B 右上角临时存放的一行（4个数）
                v5 = B[j + k][i + 4]; v6 = B[j + k][i + 5]; v7 = B[j + k][i + 6]; v8 = B[j + k][i + 7];

                // 3. 把刚才 A 左下角读出的数据，转置写入 B 的右上角
                B[j + k][i + 4] = v1; B[j + k][i + 5] = v2; B[j + k][i + 6] = v3; B[j + k][i + 7] = v4;

                // 4. 把刚才从 B 右上角拿出来的临时数据，写入 B 的左下角
                B[j + k + 4][i + 0] = v5; B[j + k + 4][i + 1] = v6; B[j + k + 4][i + 2] = v7; B[j + k + 4][i + 3] = v8;
            }

            // ==================== 最后一步 ====================
            // 处理右下角（A 的右下 -> B 的右下），这部分完全没有冲突，直接常规转置
            for (k = 0; k < 4; k++) {
                v1 = A[i + 4][j + k + 4]; v2 = A[i + 5][j + k + 4]; v3 = A[i + 6][j + k + 4]; v4 = A[i + 7][j + k + 4];
                
                B[j + k + 4][i + 4] = v1; B[j + k + 4][i + 5] = v2; B[j + k + 4][i + 6] = v3; B[j + k + 4][i + 7] = v4;
            }
            
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

