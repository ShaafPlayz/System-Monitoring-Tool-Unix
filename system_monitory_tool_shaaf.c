#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <poll.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>



int memory(int sam, long delay, int row_main, int col_main, bool run) { 
    FILE *memfile;
    memfile = fopen("/proc/meminfo", "r");
    if (memfile == NULL) {
        fprintf(stderr, "File Empty");
        return 1;
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
    double mem_util_used = mem_util_total - mem_util_available;
    fclose(memfile);
    double display_ram = ((((int)mem_util_total/10)*10)+10);
    int row = (mem_util_used/ display_ram) *10;

    // This sections runs until the program ends to update the graph with new values
    if(run == true) {
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

    // This Runs only once (to draw graph and set initial values)
    if (run == false) {
        printf("\x1B[%dG", 0);
        printf("> Memory usage: %.2f GB\n", mem_util_used);
        printf("\n");
        printf("%.0f GB   |\n", display_ram);
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
        while(samplespast < sam) {
            printf("_");
            samplespast += 1;
        }
        if(row != 0) {printf("\x1B[%dA", row);}
        printf("\x1B[%dG", col_main);
        printf("#");
        fflush(stdout);
    }
    return row;
    
}

int usecpu(int sam, long delay, int* row_main, int col_main, bool run, long* prev_total, long* prev_idle) {
    long user, nice2, sys, idle, iowait, irq, softirq;
    
    FILE *cpufile;
    cpufile = fopen("/proc/stat", "r");
    if (cpufile == NULL) {
        fprintf(stderr, "Cpu Cores File Empty");
        return 1;
    }

    fscanf(cpufile, "cpu %ld %ld %ld %ld %ld %ld %ld", &user, &nice2, &sys, &idle, &iowait, &irq, &softirq);
    
    long total_now = user + nice2 + sys + idle + iowait + irq + softirq;


    long change_total = total_now - *prev_total;
    long change_idle = idle - *prev_idle;
    
    double cpu_util = 100 * (1.0 - ((double)change_idle / (double)change_total));
    int graph_cpu = cpu_util / 10;
    int row = graph_cpu;
    int i = 0;

    // This sections runs until the program ends to update the graph with new values
    if(run == true) {
        int c_row = abs(row - *row_main);
        int location = 12;
        printf("\x1B[%dG", 0);
        printf("> CPU utilization: %.2f %%\n", cpu_util);
        location--;
        printf("\x1B[%dB", 11);
        location -= 11;
        printf("\x1B[%dA", *row_main);
        location += *row_main;
        
        if(location == 0) {
            printf("\x1B[%dG", col_main - 1);
            printf("_");
        } else {
            printf("\x1B[%dG", col_main);
        }
        if(*row_main == row){ 
            printf(":");
            fflush(stdout); 
        } else if (*row_main < row){
            if(location + c_row >= 10) {
                printf(":");
                fflush(stdout); 
            } else {
                printf("\x1B[%dA", c_row);
                printf(":");
                fflush(stdout);
            }
            
        } else if (*row_main > row){
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

    // This Runs only once (to draw graph and set initial values)
    if (run == false) {
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
        while(samplespast < sam) {
            printf("_");
            samplespast += 1;
        }
        if(row != 0){printf("\x1B[%dA", row);}
        if((row - *row_main) == 10){printf("\x1B[%dB", 1);}
        printf("\x1B[%dG", col_main);
        printf(":");
        fflush(stdout);
    }
    fclose(cpufile);
    *prev_total = total_now;
    *prev_idle = idle;
    *row_main = row;
    return 0;
}

int manycores(int last_sample, int current_sample) {
    if ((last_sample + 1) == current_sample){

        long maxfreqcpu;
        FILE *cpufreq;
        cpufreq = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq","r");
        if (cpufreq == NULL) {
            fprintf(stderr, "Cpu Max Frequency File Empty");
        }
        fscanf(cpufreq, "%ld", &maxfreqcpu);


        
        FILE *coresfile;
        coresfile = fopen("/proc/cpuinfo", "r");
        if (coresfile == NULL) {
            fprintf(stderr, "File Empty");
            return 1;
        }


        char buffer[40];
        int num_cores = 0;
        double clock_rate = maxfreqcpu / 1000000.0;
        
        while (fgets(buffer, sizeof(buffer), coresfile) != NULL) {
            if (strncmp(buffer, "processor", 9) == 0) {
                char *colon = strchr(buffer, ':');
                if (colon != NULL) {
                    int procNum = atoi(colon + 1);
                    num_cores = procNum;
                }
            }
        }
        num_cores += 1;
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
        fclose(coresfile);
        fclose(cpufreq);
        return lines;
    }
}

          

int main(int argc, char **argv) {
    int i = 1;
    int samples = 20; // Samples default value = 20
    long delay = 500000; // tdelay default value = 0.5 seconds
    
    bool cpu = false;
    bool mem = false;
    bool cores = false;
    bool pos_arg = false;
    printf("\x1B[2J");


    // The following code checks for the positional samples and tdelay
    // arguments. If found, the code will ignore --samples=N and --tdelay=T.
    if (1 < argc && isdigit(argv[1][0])) {
        samples = (int)strtol(argv[1], NULL, 10);
        pos_arg = true;

        if (2 < argc && isdigit(argv[2][0])) {
            delay = (int)strtol(argv[2], NULL, 10);
        }
    }
    
    // The following code compares the command line arguments with any known
    // arguments. Any extra/unknown commands by the user are ignored for
    // ease of user experience. (it gets annoying when you mistype something)
    while(i < argc && argc > 0) {
        if(strncmp("--samples=", argv[i], 10) == 0) {
            if(pos_arg == false) {
                strtok(argv[i], "=");
            
            char *substring = strtok(NULL, "=");
            if(substring != NULL) {
                samples = strtol(substring, NULL, 10);
            }
        } 
            
        }
        if(strncmp("--tdelay=", argv[i], 9) == 0) {
            if(pos_arg == false) {
                strtok(argv[i], "=");
                
                char *substring = strtok(NULL, "=");
                if(substring != NULL) {
                    delay = strtol(substring, NULL, 10);
                }
            }
            
        }
        if(strcmp("--memory", argv[i]) == 0) {
            mem = true;
        }
        if(strcmp("--cpu", argv[i]) == 0) {
            cpu = true;
        }
        if(strcmp("--cores", argv[i]) == 0) {
            cores = true;
        }
        i++;
    }


    // We open cpu file to sample the first cpu readings
    FILE *cpufile;
    cpufile = fopen("/proc/stat", "r");
    if (cpufile == NULL) {
        fprintf(stderr, "File Empty");
        return 1;
    }
    
    long user, nice2, sys, idle = 0, iowait, irq, softirq;
    long total = 0;
    long idle_passed;
    bool did_we_run = false;

    long milidelay = delay/1000;
    int col_main = 10;
    int time = 0;
    int row_mem = 0;
    int row_cpu = 0;
    int lines = 0;
    printf("\x1B[H");
    printf("Nbr of samples: %d -- every %ld microSecs(%.1f secs)", samples, delay, (double)delay/1000000.0);
    printf("\x1B[2B");
    printf("\x1B[s");

    // if use selects all functions to be displayed, we default them 
    // to all false, because of the way I coded
    if(mem == true && cpu == true && cores == true) {
        mem = false;
        cpu = false;
        cores = false;
    }
    
    // The following code decides what components are to be displayed
    // The following code is also responsible for updating the values on a program
    while(time < samples) {
        // Now we check to run all diagnostics
        if(mem == false && cpu == false && cores == false) {
            //run all commands if values untouched
            printf("\x1B[u");
            row_mem = memory(samples, delay, row_mem, col_main + time, did_we_run);
            printf("\x1B[u");
            printf("\x1B[13B");
            usecpu(samples, delay, &row_cpu, col_main + time, did_we_run, &total, &idle_passed);
            printf("\x1B[u");
            printf("\x1B[13B");
            printf("\x1B[13B");
            lines = manycores(time, samples);
        } 
        if (mem == true) {
            printf("\x1B[u");
            row_mem = memory(samples, delay, row_mem, col_main + time, did_we_run);
        } if (cpu == true) {
            printf("\x1B[u");
            if(mem == true) {printf("\x1B[13B");}
                usecpu(samples, delay, &row_cpu, col_main + time, did_we_run, &total, &idle_passed);
        } if (cores == true) {
            printf("\x1B[u");
            if(mem == true || cpu == true) {printf("\x1B[13B");}
            else {time = 19;}
            lines = manycores(time, samples);
        }
        time += 1;
        did_we_run = true;
        col_main += 0.9;
        poll(NULL, 0, milidelay);
    }

    fclose(cpufile);

    // The following code decides the final cursor position after
    // all functions have been run.
    printf("\x1b[u");
    printf("\x1B[%dG", 0);
    lines = lines*4;
    if(mem == false && cpu == false && cores == false) {
        printf("\x1b[%dB", 26 + lines + 1);
    } 
    if (mem == true) {
        printf("\x1b[13B");
    } if (cpu == true) {
        printf("\x1b[13B");
    } if (cores == true) {
        printf("\x1b[%dB", lines + 1);
    }
    return 0;
}

