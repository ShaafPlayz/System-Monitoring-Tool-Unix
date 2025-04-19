#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <unistd.h>

#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <sys/select.h>

#include <signal.h>
#include "memory.h"
#include "cpu.h"
#include "cores.h"


/* The following Function handles the CTRL + C signal from the user
    and prompts him to either confirm or cancel ending the program
*/
volatile sig_atomic_t menupopup = 0;

void handler(int signal){
    menupopup = 1;
}

int menu() {
    // char buffer;
    printf("\x1b[0E");
    printf(" ==============================================\n");
    printf("| Would You Like to END the Program?           |\n");
    printf("|     _____________       _____________        |\n");
    printf("|     |  TO EXIT  |       | TO RESUME |        |\n");
    printf("|     |  type > 1 |       |  type > 0 |        |\n");
    printf("|     |___________|       |___________|        |\n");
    printf(" ==============================================\n");
    
    int a;
    scanf("%d", &a);
    printf("\x1b[8F");
    printf("\x1b[0J");
    if (a == 1){
        return 1; 
    } else if (a == 0){
        return 0;
    } else {
        return 0;
    }
}


int main(int argc, char **argv) {
    // CTRL + C (uses handler Function)
    struct sigaction Csignal;
    Csignal.sa_handler = handler;
    Csignal.sa_flags = SA_RESTART;
    sigemptyset(&Csignal.sa_mask);
    sigaction(SIGINT, &Csignal, NULL);
    
    fflush(stdout);
    
    // ==========================================
    // CTRL + Z (ignores)
    // struct sigaction Z;
    // Z.sa_handler = SIG_IGN;
    // Z.sa_flags = 0;
    // sigemptyset(&Z.sa_mask);
    // sigaction(SIGTSTP, &Z, NULL);


    signal(SIGTSTP, SIG_IGN);

    
    fflush(stdout);
    // ==========================================
    

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
        if(strncmp("--nsam=", argv[i], 10) == 0) {
            if(pos_arg == false) {
                strtok(argv[i], "=");
            
            char *substring = strtok(NULL, "=");
            if(substring != NULL) {
                samples = strtol(substring, NULL, 10);
            }
        } 
            
        }
        if(strncmp("--ninterval=", argv[i], 9) == 0) {
            if(pos_arg == false) {
                strtok(argv[i], "=");
                
                char *substring = strtok(NULL, "=");
                if(substring != NULL) {
                    delay = strtol(substring, NULL, 10);
                }
            }
            
        }
        if(strcmp("--umem", argv[i]) == 0) {
            mem = true;
        }
        if(strcmp("--ucpu", argv[i]) == 0) {
            cpu = true;
        }
        if(strcmp("--ucores", argv[i]) == 0) {
            cores = true;
        }
        i++;
    }
    // If no flags are selected, we default them  
    // to all true, because of the way I coded
    if(mem == false && cpu == false && cores == false) {
        mem = true;
        cpu = true;
        cores = true;
    }


    // We open cpu file to sample the first cpu readings
    FILE *cpufile;
    cpufile = fopen("/proc/stat", "r");
    if (cpufile == NULL) {
        fprintf(stderr, "File Empty");
        return 1;
    }
    
    // Parent Process Variables
    bool run = false;
    long milidelay = delay/1000;
    int col_main = 10;
    int time = 0;
    int lines = 0;
    printf("\x1B[H");
    printf("Nbr of samples: %d -- every %ld microSecs(%.1f secs)", samples, delay, (double)delay/1000000.0);
    printf("\x1B[2B");
    printf("\x1B[s");

   
    pid_t memPID = -1;
    pid_t cpuPID = -1;
    pid_t corePID = -1;

    // Memory Variables
    int row_mem = 0;
    double mem_util_used, display_ram;
    int last_row_mem;

    // CPU Variables
    int row_cpu = 0;
    int last_row_cpu;
    float cpu_util = 0;
    long user, nice2, sys, idle = 0, iowait, irq, softirq;
    long total = 0;
    long idle_passed;
    

    // Cores Variables
    int num_cores;
    double clock_rate;


    //=================================================================
    // Open Pipes and create buffers
    int toMain_MEM[2], time_Main[2], time_Main2[2],toMain_CPU[2], fromCores[2];

    pipe(time_Main);
    pipe(time_Main2);
    pipe(toMain_MEM);
    pipe(toMain_CPU);
    pipe(fromCores);

    char buffer[300]; // Buffer We use to get back Data
    fflush(stdout);
    //==================================================


    // Create Concurrent Processes
    if(mem == true){
        memPID = fork();
        if(memPID == 0){
            
            char timeBuf[100];
            int memTime;
            close(toMain_MEM[0]); // close read for SENDING DATA TO MAIN pipe
            close(time_Main[1]);  // close write to RECEIVING DATA FROM MAIN pipe
            
            // Foreign Pipes we can Close
            close(time_Main2[0]);
            close(toMain_CPU[0]);
            close(fromCores[0]);
            close(time_Main2[1]);
            close(toMain_CPU[1]);
            close(fromCores[1]);
            //++++++++++++++++++++++++++
            
            while(read(time_Main[0], timeBuf, 100) > 0){     // read DATA from MAIN
                memTime = atoi(timeBuf);                     // convert DATA to INTEGER
                if(memTime == samples) {break;}              // Finish Marker <=

                dataMem(&row_mem, &mem_util_used, &display_ram); // reading memeory data and save
                sprintf(buffer, "%d %f %f", row_mem, mem_util_used, display_ram);  // save data as string to buffer
                write(toMain_MEM[1], buffer, sizeof(buffer));                                   // send to pipe
                //printf("disp: %f, row: %d, mem used: %f \n", display_ram, row, mem_util_used);
            }
            close(toMain_MEM[1]);
            exit(1);
        } 
    }
    
    
    
    if(cpu == true){
        cpuPID = fork();
        if(cpuPID == 0){
            char timeBuf[100];
            int cpuTime;
            close(toMain_CPU[0]); // close read for SENDING DATA TO MAIN pipe
            close(time_Main2[1]);  // close write to RECEIVING DATA FROM MAIN pipe

            // Foreign Pipes we can Close
            close(time_Main[0]);
            close(toMain_MEM[0]);
            close(fromCores[0]);
            close(time_Main[1]);
            close(toMain_MEM[1]);
            close(fromCores[1]);
            //++++++++++++++++++++++++++
            dataCPU(&total, &idle_passed);

            while(read(time_Main2[0], timeBuf, 100) > 0){     // read DATA from MAIN
                cpuTime = atoi(timeBuf);                     // convert DATA to INTEGER
                if(cpuTime == samples) {break;}              // Finish Marker <=
                printf("\x1B[u");
                printf("\x1B[13B");
                printf("\x1B[13B");
                printf("\x1B[%dG", 0);
                
                row_cpu = calculateCPU(&total, &idle_passed, &cpu_util); // reading cpu data and save

                sprintf(buffer, "%ld %ld %d %f", total, idle_passed, row_cpu, cpu_util);  // save data as string to buffer
                write(toMain_CPU[1], buffer, sizeof(char)*300); 
            }
            close(toMain_MEM[1]);
            exit(1);
        }
    }
    
    if(cores == true){
        corePID = fork();
        if(corePID == 0) {
            close(fromCores[0]); // close Read PIPE for sending lines to Main
            // Close Foreign Pipes
            // closing these pipes are probably unecessary since
            // the program just exits really fast but i am still gonna do it :p
            close(time_Main2[0]);
            close(toMain_CPU[0]);
            close(time_Main2[1]);
            close(toMain_CPU[1]);
            close(time_Main[0]);
            close(toMain_MEM[0]);
            close(time_Main[1]);
            close(toMain_MEM[1]);
            //++++++++++++++++++++++++++
            dataCores(&num_cores, &clock_rate);

            sprintf(buffer, "%d %lf", num_cores, clock_rate);  // save data as string to buffer
            write(fromCores[1], buffer, sizeof(char)*300);
        
            close(fromCores[1]);
            exit(1);
        }
    }
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ++++++++++++++++++++++++MAIN FUNCTION+++++++++++++++++++++++
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    
    // ==========================================
    // Close Unused Pipes
    close(toMain_MEM[1]); // close write for MEMORY pipe
    close(toMain_CPU[1]); // close write for CPU pipe
    close(time_Main[0]);  // close read for SENDING TIME DATA pipe
    close(time_Main2[0]);  // close read for SENDING TIME DATA pipe
    close(fromCores[1]);   // close write for Cores Pipe
    // ==========================================
    char cpuBuf[300], timeBuf[100], coresBuf[300];
    // Send the first Time check
    sprintf(timeBuf, "%d", time);
    if(mem == true){
        write(time_Main[1], &timeBuf,  sizeof(timeBuf));
    }
    if(cpu == true){
        write(time_Main2[1], &timeBuf,  sizeof(timeBuf));
    }


    // check if only cores is asked
    if(mem == true || cpu == true){
        while(time < samples) {
            // Signal Handling Function
            if(menupopup == 1){
                menupopup = 0;
                printf("\x1B[u");
                if (mem == true) {
                    printf("\x1b[13B");
                } 
                if (cpu == true) {
                    printf("\x1b[13B");
                } 

                if(menu() == 1) {
                    if(mem == true){
                        kill(memPID, SIGKILL);
                    }
                    if(cpu == true){
                        kill(cpuPID, SIGKILL);
                    }
                    if(cores == true){
                        kill(corePID, SIGKILL);
                    }
                    return 0;
                }
            }
            
            // Prepare Data for Memory Plotting
            if(mem == true){
                read(toMain_MEM[0], buffer, sizeof(buffer));
                sscanf(buffer, "%d %lf %lf", &row_mem, &mem_util_used, &display_ram);
            }
            // Prepate Data for CPU Plotting
            if(cpu == true){
                read(toMain_CPU[0], cpuBuf, sizeof(char)*300);  // read Cpu Data
                sscanf(cpuBuf, "%ld %ld %d %f", &total, &idle_passed, &row_cpu, &cpu_util); 
            }
            
            
            if(time == 0){ // Draw Graphs
                // MEMORY
                if(mem == true){
                    printf("\x1B[u");
                    last_row_mem = 0;
                    conceiveGraMem(samples, row_mem, mem_util_used, display_ram);
                    fflush(stdout);
                }
                // CPU
                if(cpu == true){
                    printf("\x1B[u");
                    if(mem == true){
                        printf("\x1B[13B");
                    }
                    last_row_cpu = 0;
                    conceiveGraCPU(0, 0, samples, 0);
                    fflush(stdout);
                }
                
            } else {
                // Memory Plotting
                if(mem == true){
                    printf("\x1B[u");
                    plotMem(last_row_mem, col_main + time, row_mem, mem_util_used);  // plot mem graph
                    last_row_mem = row_mem;
                    fflush(stdout);
                }
                // CPU Plotting
                if(cpu == true){
                    printf("\x1B[u");
                    if(mem == true){
                        printf("\x1B[13B");
                    }
                    plotCPU(last_row_cpu, row_cpu, col_main + time, cpu_util);       // plot cpu graph
                    
                    last_row_cpu = row_cpu;
                    fflush(stdout);
                }
            }
            
            
            col_main += 0.9;
            usleep(delay);
            time += 1;
            // update time to make child work
            sprintf(timeBuf, "%d", time);
            if(mem == true){
                write(time_Main[1], &timeBuf,  sizeof(timeBuf));
            }
            if(cpu == true){
                write(time_Main2[1], &timeBuf,  sizeof(timeBuf));
            }
        }
    }
    if(cores == true){
        read(fromCores[0], coresBuf, sizeof(char)*300);  // read Cores Data
        sscanf(coresBuf, "%d %lf", &num_cores, &clock_rate);
        printf("\x1B[u");
        if (mem == true) {
            printf("\x1b[13B");
        } 
        if (cpu == true) {
            printf("\x1b[13B");
        } 
        lines = graphCores(num_cores, clock_rate);
    }
    
    
    close(toMain_MEM[0]); // close MEMORY pipe
    close(toMain_CPU[0]); // close CPU pipe
    close(time_Main[1]);  // close TIME pipe for MEM
    close(time_Main2[1]);  // close TIME pipe for CPU
    close(fromCores[0]);  // close CORE pipe

    if(mem == true){
        kill(memPID, SIGKILL);
    }
    if(cpu == true){
        kill(cpuPID, SIGKILL);
    }
    if(cores == true){
        kill(corePID, SIGKILL);
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