#include "cachelab.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
// @Adan //

int global_counter = 0;
int verbose = 0;
int opt;
int s, E, b;
cache_line **cache = NULL;
int hit_count = 0, miss_count = 0, eviction_count = 0;

void parse_trace(char *trace_file);
void access_cache(unsigned long address);


typedef struct 
{
    int valid;
    int tag;
    int time_stamp;
} cache_line;

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
    cache_line **cache = (cache_line **) malloc(S * sizeof(cache_line *));
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
        if (operation == "I") {
            continue;
        }

        if (verbose == 1) {
            printf("%c %lx,%d ", operation, address, size);
        }

        
    }
}

void access_cache(unsigned long address) {
    
}