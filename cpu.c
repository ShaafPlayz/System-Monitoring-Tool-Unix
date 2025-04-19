#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <poll.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <sys/param.h>
#include "cpu.h"

///_|> descry: Gets CPU data from /proc/
///_|> total: stores total cpu time
///_|> idle2: stores the idle cpu time
///_|> returning: void function returns nothing
void dataCPU(long* total, long* idle2){
    long user, nice2, sys, idle, iowait, irq, softirq;
    
    FILE *cpufile;
    cpufile = fopen("/proc/stat", "r");
    if (cpufile == NULL) {
        fprintf(stderr, "Cpu Cores File Empty");
        *total = 0;
        *idle2 = 0;
        return;
    }

    fscanf(cpufile, "cpu %ld %ld %ld %ld %ld %ld %ld", &user, &nice2, &sys, &idle, &iowait, &irq, &softirq);
    long total_now = user + nice2 + sys + idle + iowait + irq + softirq;
    *total = total_now;
    *idle2 = idle;
    fclose(cpufile);
}

///_|> descry: Calculates CPU utilization using formulas and data from dataCPU
///_|> prev_total: stores previous total cpu time
///_|> prev_idle: stores previous idle cpu time
///_|> cpu_util:  stores the CPU utilization in % out of 100
///_|> returning: returns the row number for plotting on graph
int calculateCPU(long* prev_total, long* prev_idle, float* cpu_util){
    long total_now, idle;

    dataCPU(&total_now, &idle); // Calculate Statistics Now

    long change_total = total_now - *prev_total;
    long change_U = (total_now - idle) - (*prev_total - *prev_idle);
    if(change_total == 0){
        *cpu_util = 0;
    } else {
        *cpu_util = 100 * (((double)change_U / (double)change_total));
    }
    
    int graph_cpu = *cpu_util / 10;
    int row = graph_cpu;
    
    *prev_total = total_now;
    *prev_idle = idle;
    return row;
}

///_|> descry: updates graph for CPU with new data
///_|> row_main: previous row printed
///_|> row: current row about to be printed
///_|> col_main: column to print the data at
///_|> cpu_util: stores the CPU utilization in % out of 100
///_|> returning: void function returns nothing
void plotCPU(int row_main, int row, int col_main, float cpu_util) {
    int c_row = abs(row - row_main);
    int location = 12;
    printf("\x1B[%dG", 0);
    printf("> CPU utilization: %.2f %%\n", cpu_util);
    location--;
    printf("\x1B[%dB", 11);
    location -= 11;
    printf("\x1B[%dA", row_main);
    location += row_main;
    
    if(location == 0) {
        printf("\x1B[%dG", col_main - 1);
        printf("_");
    } else {
        printf("\x1B[%dG", col_main);
    }
    if(row_main == row){ 
        printf(":");
        fflush(stdout); 
    } else if (row_main < row){
        if(location + c_row >= 10) {
            printf(":");
            fflush(stdout); 
        } else {
            printf("\x1B[%dA", c_row);
            printf(":");
            fflush(stdout);
        }
        
    } else if (row_main > row){
        if(location - c_row < 1) {
            printf(":");
            fflush(stdout); 
        } else {
            printf("\x1B[%dB", c_row);
            printf(":");
            fflush(stdout); 
        }
        
    }
        
}

///_|> descry: draws empty graph for CPU
///_|> samplesNow: the sample main function is at
///_|> row: current row about to be printed
///_|> row_main: previous row stores in Main function
///_|> cpu_util: stores the CPU utilization in % out of 100
///_|> returning: void function returns nothing
void conceiveGraCPU(int row, int row_main, int samplesNow, float cpu_util){
    // This Runs only once (to draw graph and set initial values)
    printf("\x1B[%dG", 0);
    printf("> CPU utilization: %.2f %%\n", cpu_util);
    
    printf("\n");
    printf("100 %%   |\n");
    printf("        |\n");
    printf("        |\n");
    printf("        |\n");
    printf("        |\n");
    printf("        |\n");
    printf("        |\n");
    printf("        |\n");
    printf("        |\n");
    printf("  0 %%   |");

    int samplespast = 0;
    while(samplespast < samplesNow) {
        printf("_");
        samplespast += 1;
    }
    if(row != 0){printf("\x1B[%dA", row);}
    if((row - row_main) == 10){printf("\x1B[%dB", 1);}
    printf("\x1B[%dG", 10);
    printf(":");
    fflush(stdout);
}