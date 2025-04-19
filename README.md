# System Monitoring Tool - Concurrency & Signals (Command Line Tool)
## *1. Metadata*
*Muhammad Shaaf Yousaf* \
*7th April 2025*

## *2. Introduction*
This program is written in C language and it displays **UNIX** system information in real time using graphs.
With this program, you can view the following system information:
- Memory Usage (in GB)
- CPU Utilization (in %)
- Number of CPU cores and max CPU frequency (in GHz)

All three functions above run in concurrency to the main process.

## *3. Approach behind the implementation*
Displaying system information to the user was the point of this program. However, the concurrency requirement required a creative approach. From a high level, this program reads system information from the following UNIX files using multiple child proccesses:
- /proc/meminfo
- /proc/stat
- /proc/cpuinfo
- /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq

The graphs in the terminal are all updated using **ANSI** escape sequences.
You need to have a terminal that supports **ANSI** escape sequences, and access to a **UNIX** system, and **GCC** (or any alternative C compiler).
## *4. Implementation*
### 4.1: Idea
 This program creates ***three child processes*** that reads system files to get data for ```cpu usage```, ```memory usage```, and ```cores frequency and count```. The parent process gets all this data through ***Pipes***, which it then usess to graph all this data on nicely made **ASCII** graphs in the terminal.


### 4.2: Modules
There are 3 modules, which are as below:
- memory.c: ```void dataMem()```, ```void plotMem()```, ```void conceiveGraMem()```
- cpu.c: ```void dataCPU()```, ```int calculateCPU()```, ```void plotCPU()```, ```void conceiveGraCPU()```
- cores.c: ```void dataCores()```, ```int graphCores()```


Each module has a ```data``` function, a ```graphing``` function, with the cpu containing an extra ```calculate``` function to make sense of the data stored.

### 4.3: Inner Workings
Functions inside ```memory.c```:
```C
  void dataMem(); // The function reads "/proc/meminfo" 
                  // and extracts MemTotal, MemFree, MemAvailable, Buffers, Cached
  void plotMem(); // This function plots the graph with data acqured by dataMem()
  void conceiveGraMem(); // This function draws an empty graph, and is called before using the plotMem() function
  ```
Functions inside ```cpu.c```:
```C
  void dataCPU(); // This function gets its N-1 Total CPU Time, and idle_time query from the Main Fuction 
                  // (where it stores the time back to for use in the next cycle).
                  // When the function runs, it reads "/proc/stat" and extracts the 
                  // cpu times -> user, nice, sys, idle, iowait, irq, softirq.
                  // The function then saves the Total CPU Time by adding all of the cpu times above 
                  // into a variables
  int calculateCPU(); // The function calculates CPU Utilization by using the information from dataCPU
                      // It gets data at time N and at time N -1 and uses the formula (Change in U)/(Change in T)
                      // Where U_i = T_i - I_i and T = T_i - T_i-1
                      // The function calculates the row by dividing "Total CPU Utilization"/ 10.
  void plotCPU(); // This function plots the graph with data acqured by dataCPU()
  void conceiveGraCPU(); // This function draws an empty graph, and is called before using the plotCPU() function
  ```
Functions inside ```cores.c```:
```C
  void dataCores(); // The function reads the maximum cpu frequency from the 
                    // file"/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq" 
                    // It also calculates the number of CPU Cores from "/proc/cpuinfo". 
                    // Due to the way my program reads the file, it will add 1 to the number of cores.
  int graphCores(); // This function prints Max Cpu Frequency in words with number of cores and also
                    // it now prints a number of boxes not more than the total number of cores we calculated.
  ```
All the modules above run concurrently as Child processes and they communicate to the Parent process using pipes.

#### Role of the Main Function:
The Main function acts as a Parent Process for all of these comands to work as intended.

The Main function calculates how much time is left and draws and updates the graph with data sent through pipes from the child processes.

## *5. Program FlowChart*
![alt text](flowchara3.png)

## *6. Instructions on Compiling*
Using the makefile, we can compile the code using ```make``` command in the terminal.\
We can also use ```make run``` to run our program, but this would cause our signals to not work as expected.\
We can also use ```make clean``` to delete all files produced by make including the executable.\
If we instead would like to compile the program without makefile, we can use the following command: \
```C
gcc -std=gnu99 -o program.out main.c memory.c cores.c cpu.c
```

## *7. Expected Results*
The program accepts the following command-line arguments:

***By Default, if  no arguemnts are provided, our program will run ```--memory```, ```--cpu```, ```--cores``` and will display all currently running processes.***
You have access to the following command line arguments (Flags) that the program accepts:
- ```--cpu``` (Displays CPU Utilization Graph in %)
- ```--memory``` (Displays Memory Usage Graph in GB)
- ```--cores``` (Returns Number of CPU Cores along with Max CPU Frequency)
- ```--samples=N``` (If used the value **N** will indicate how many statistics are collected)
- ```--tdelay=T``` (If used the value **T** will indicate how frequently to collect satatistics)

In addition to above Command Line Graphs, we can use the following keyboard shortcuts to control the program runtime:
- ```CTRL + C``` This shortcut would suspend our program and will ask the user if they would like to EXIT or RESUME the program.
- ```CTRL + Z``` This shortcut would usually terminate any running program on a UNIX system, but for this program I have set this shortcut to be ignored by the program.

## *8. Test Cases*
***8.1: General Cases***


The program will run as many recognized arguments: 

- ```./program.out 50 600000 --cpu --tdelay=100```

The program sees "--cpu" command above, this indicates that the program will now only display the CPU graph. Observe that "50" is read as the number of samples, "600000" is read as tdelay in microseconds, and the extra "--tdelay=100" command is ignored. 

- ```./program.out --memory efwfwefewf```
- ```./main.out efwfwefewf```

In the arugments provided above, the program will ignore the gibrish and run the the recognized commands within them (eg: ```--memory```).

## *9. Disclaimer*

I had a sitting with Marcelo, and we discovered that the CPU Utilization formula that we were provided actually gives us the total CPU Utilization for the entire system. So its not just limited to the user that we have signed in to. 

If we were to remove the ```sys``` cpu time from our calculations then we would indeed get the CPU time for the current user. However, Marcelo asked me to write this in the disclaimer in the ```README```.



## *10. References*
- https://man7.org/linux/man-pages/man5/proc.5.html
- https://www.tutorialspoint.com/c_standard_library/c_function_fwrite.htm
- https://docs.kernel.org/filesystems/proc.html

