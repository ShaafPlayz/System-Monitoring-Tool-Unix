#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <poll.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include "memory.h"

///_|> descry: Gets MEMORY data from /proc/
///_|> row: stores row used by graph
///_|> mem_util_used: stores MEMORY used
///_|> display_ram:  stores the amount to be displayed
///_|> returning: void function returns nothing
void dataMem(int *row, double *mem_util_used, double *display_ram) {
    FILE *memfile;
    memfile = fopen("/proc/meminfo", "r");
    if (memfile == NULL) {
        fprintf(stderr, "File Empty");
        *mem_util_used = 0;
        *display_ram = 0;
        *row = 0;
        return;
    }
    long infomem [5];
    long memtotal, memfree, memava, membuf, memcach;
    
    fscanf(memfile, "MemTotal: %ld kB", &memtotal);
    fgetc(memfile);
    fscanf(memfile, "MemFree: %ld kB", &memfree);
    fgetc(memfile);
    fscanf(memfile, "MemAvailable: %ld kB", &memava);
    fgetc(memfile);
    fscanf(memfile, "Buffers: %ld kB", &membuf);
    fgetc(memfile);
    fscanf(memfile, "Cached: %ld kB", &memcach);
    
    double mem_util_total = memtotal /1024.0 /1024.0;
    double mem_util_available = memava /1024.0 /1024.0;
    *mem_util_used = mem_util_total - mem_util_available;
    fclose(memfile);
    *display_ram = ((((int)mem_util_total/10)*10)+10);
    *row = (*mem_util_used/ *display_ram) *10;
}

///_|> descry: updates graph for MEMORY with new data
///_|> row_main: previous row printed
///_|> row: current row about to be printed
///_|> col_main: column to print the data at
///_|> mem_util_used: has current MEMORY used data
///_|> returning: void function returns nothing
void plotMem(int row_main, int col_main, int row, double mem_util_used) { 
    // This sections runs until the program ends to update the graph with new values
    int location = 11;
    printf("\x1B[%dG", 0);
    printf("> Memory usage: %.2f GB\n", mem_util_used);
    location--;
    printf("\x1B[%dB", 10);
    location -= 10;
    if(row_main != 0){
        printf("\x1B[%dA", row_main);
        location += row_main;
    }
    if (location == 0) {
        printf("\x1B[%dG", col_main - 1);
        printf("_");
    } else {
        printf("\x1B[%dG", col_main);
    }
    if(row_main == row){ 
        printf("#");    
        fflush(stdout); 
    } else if (row_main < row){
        printf("\x1B[%dA", row - row_main);
        printf("#");
        fflush(stdout); 
    } else if (row_main > row){
        printf("\x1B[%dB", row_main - row);
        printf("#");
        fflush(stdout); 
    }  
}

///_|> descry: draws empty graph for MEMORY
///_|> samplesNow: the sample main function is at
///_|> row: current row about to be printed
///_|> mem_util_used: has current MEMORY used data
///_|> display_ram: ram amount to be displayed
///_|> returning: void function returns nothing
void conceiveGraMem(int samplesNow, int row, double mem_util_used, double display_ram) {
    //This Runs only once (to draw graph and set initial values)
    fflush(stdout);
    printf("\x1B[%dG", 0);
    printf("> Memory usage: %.2f GB\n", mem_util_used);
    printf("\n");
    printf("%.0f GB\t|\n", display_ram);
    printf("        |\n");
    printf("        |\n");
    printf("        |\n");
    printf("        |\n");
    printf("        |\n");
    printf("        |\n");
    printf("        |\n");
    printf("        |\n");
    printf(" 0 GB   |");
    int samplespast = 0;
    while(samplespast < samplesNow) {
        printf("_");
        samplespast += 1;
    }
    if(row != 0) {printf("\x1B[%dA", row);}
    printf("\x1B[%dG", 10);
    printf("#");
    fflush(stdout);
}