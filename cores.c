#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <poll.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include "cores.h"

///_|> descry: Gets CORES data from /sys/
///_|> num_cores: stores total number of cores the system has
///_|> clock_rate: stores the clock rate of the cpu
///_|> returning: void function returns nothing
void dataCores(int* num_cores, double* clock_rate) {
    long maxfreqcpu;
    FILE *cpufreq;
    cpufreq = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq","r");
    if (cpufreq == NULL) {
        fprintf(stderr, "Cpu Max Frequency File Empty");
        *clock_rate = 0;
        *num_cores = 0;
        return;
    }
    fscanf(cpufreq, "%ld", &maxfreqcpu);


    
    FILE *coresfile;
    coresfile = fopen("/proc/cpuinfo", "r");
    if (coresfile == NULL) {
        fprintf(stderr, "File Empty");
        *num_cores = 0;
        return;
    }


    char buffer[40];
    *num_cores = 0;
    *clock_rate = maxfreqcpu / 1000000.0;
    
    while (fgets(buffer, sizeof(buffer), coresfile) != NULL) {
        if (strncmp(buffer, "processor", 9) == 0) {
            char *colon = strchr(buffer, ':');
            if (colon != NULL) {
                int procNum = atoi(colon + 1);
                *num_cores = procNum;
            }
        }
    }
    *num_cores += 1;
    fclose(coresfile);
    fclose(cpufreq);
}

///_|> descry: Draws a number of BOXES to show the number of CORES in the system
///_|> num_cores: stores total number of cores the system has
///_|> clock_rate: stores the clock rate of the cpu
///_|> returning: returns the lines to skip when exiting the main function
int graphCores(int num_cores, double clock_rate) {
    
    printf("\x1B[%dG", 0);
    printf("> Number of Cores: %d @ %.2f GHz\n", num_cores, clock_rate);
    int printed = 0;
    int lines = 0;
    int remaining = 0;
    
    while(printed < num_cores) {
        remaining = num_cores - printed;
        if(remaining >=4) {
        printf("+----+   +----+   +----+   +----+\n");
        printf("|    |   |    |   |    |   |    |\n");
        printf("+----+   +----+   +----+   +----+\n");
        printf("\n");
        printed += 4;
        } else if(remaining == 3){
        printf("+----+   +----+   +----+\n");
        printf("|    |   |    |   |    |\n");
        printf("+----+   +----+   +----+\n");
        printed += 3;
        } else if(remaining == 2){
        printf("+----+   +----+\n");
        printf("|    |   |    |\n");
        printf("+----+   +----+\n");
        printed += 2;
        } else if(remaining == 1){
        printf("+----+\n");
        printf("|    |\n");
        printf("+----+\n");
        printed += 1;
        }
        lines++;
    }
    return lines;
}

          
