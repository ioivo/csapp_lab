#include "cachelab.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
// @Adan //

typedef struct 
{
    int valid;
    int tag;
    int time_stamp;
} cache_line;

int global_counter = 0;
int verbose = 0;
int opt;
int s, E, b;
cache_line **cache = NULL;
int hit_count = 0, miss_count = 0, eviction_count = 0;

void parse_trace(char *trace_file);
int access_cache(unsigned long address);


int main(int argc, char *argv[])
{
    // 用 getopt 解析出 s, E, b, trace_file //
    char *trace_file = NULL;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt)
        {
        case 'h':
            printf("Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
            break;
        case 'v' :
            verbose = 1;
            break;
        case 's' :
            s = atoi(optarg);
            break;
        case 'E' :
            E = atoi(optarg);
            break;
        case 'b' :
            b = atoi(optarg);
            break;
        case 't' :
            trace_file = optarg;
        default:
            break;
        }
    }
    
    // 动态分配二维数组 //
    int S = 1UL << s;
    cache = (cache_line **) malloc(S * sizeof(cache_line *));
    if (cache == NULL) {
        printf("Malloc failed");
        exit(1);
    }

    for (int i = 0; i < S; ++i) {
        cache[i] = (cache_line *) malloc(E * sizeof(cache_line));
        if (cache[i] == NULL) {
            printf("Malloc failed");
            exit(1);
        }
    }

    for (int j = 0; j < S; ++j) {    // 初始化 //
        for (int k = 0; k < E; ++k) {
            cache[j][k].tag = 0;
            cache[j][k].time_stamp = 0;
            cache[j][k].valid = 0;
        }
    }

    //处理文件并打印相关信息
    parse_trace(trace_file);

    //释放内存
    for (int i = 0; i < S; ++i) {
        free(cache[i]);
    }
    free(cache);

    //printSummary
    printSummary(hit_count, miss_count, eviction_count);
}


void parse_trace(char *trace_file) {
    FILE *file = fopen(trace_file, "r");
    if (file == NULL) {
        perror("Fail to open file");
        exit(1);
    }

    char operation;
    unsigned long address;
    int size;

    while (fscanf(file, " %c %lx,%d", &operation, &address, &size) == 3) {
        if (operation == 'I') {
            continue;
        }

        if (verbose) {
            printf("%c %lx,%d ", operation, address, size);
        }

        int res = 3;
        int res1 = 3;
        if (operation == 'L' || operation == 'S') {
            res = access_cache(address);
        }
        else {
            res1 = access_cache(address);
            access_cache(address);
        }
        if (verbose){
            switch (res)
            {
            case 1:
                printf("hit\n");
                break;
            case 0 :
                printf("miss\n");
                break;
            case 2 :
                printf("miss eviction\n");
                break;
            default:
                break;
            }
        }
        if (res != 3) continue;

        if (verbose) {
            switch (res1)
            {
            case 1:
                printf("hit hit\n");
                break;
            case 0 :
                printf("miss hit\n");
                break;
            case 2 :
                printf("miss eviction hit\n");
                break;
            default:
                break;
            }
        }
    }
    fclose(file);
}

int access_cache(unsigned long address) { // 0-miss, 1-hit, 2-eviction
    global_counter++;

    // 提取 set 和 tag //
    address = address >> b;
    int mask = (1U << s) - 1; // 低s位全1
    int set = (int) (address & mask);
    int tag = (int) (address >> s);

    cache_line *lines = cache[set];
    int is_miss = 1;
    int is_evi = 1;
    for (int i = 0; i < E; ++i) {
        if (lines[i].valid == 1 && lines[i].tag == tag) {
            hit_count++;
            is_miss = 0;
            is_evi = 0;
            lines[i].time_stamp = global_counter;
            return 1;
        }
    }

    int res = 0;
    if (is_miss) {
        miss_count++;
        for (int j = 0; j < E; ++j) {
            if (lines[j].valid == 0) {
                lines[j].tag = tag;
                lines[j].valid = 1;
                lines[j].time_stamp = global_counter;
                is_evi = 0;
                break;
            }
        }
    }

    if (is_evi) {
        res = 2;
        eviction_count++;

        // 找出LRU
        int LRU_index = 0;
        for (int k = 0; k < E; ++k) {
            if (lines[k].time_stamp < lines[LRU_index].time_stamp) {
                LRU_index = k;
            }
        }

        lines[LRU_index].tag = tag;
        lines[LRU_index].time_stamp = global_counter;
    }

    return res;
}