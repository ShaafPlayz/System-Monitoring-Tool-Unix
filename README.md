# System Monitoring Tool for UNIX based systems
<img width=500 src="https://github.com/user-attachments/assets/4c78568f-c61b-46e7-a205-5b3922d52061">


## Commands
You have access to the following command line arguments (Flags) that the program accepts:
- "--ucpu" (Displays CPU Utilization Graph in %)
- "--umem" (Displays Memory Usage Graph in GB)
- "--ucores" (Returns Number of CPU Cores along with Max CPU Frequency)
- "--nsam=N" (If used the value **N** will indicate how many statistics are collected)
- "--ninterval=T" (If used the value **T** will indicate how frequently to collect satatistics)

**It should be noted that the DEFAULT value of samples = 20 and tdelay = 500000 microseconds**

**It should also be noted that by DEFAULT all statistics are displayed (Memory, CPU, Cores). If a command line argument is passed to display a specific statistic, then the program defaults to only displaying that specific statistic**

**Number of samples and tdelay are also accepted in the format: "./myMonitoringTool [N [T]]"**

## Example Usage of Commands
Take as example:**./myMonitoringTool 50 600000 --cpu --tdelay=100**

Note that "50" is read as the number of samples, "600000" is read as tdelay in microseconds, and the extra "--tdelay=100" command is ignored.

We also observe the "--cpu" command, this indicates that the program will now only display the CPU graph.


## How to use?
- Compile the code, use the following command in a **UNIX** terminal:
```
gcc system_monitory_tool_shaaf.c --std=c99 -o myMonitoringTool
```
- Run the Program, use the following command in a **UNIX** terminal: (This runs the program using its DEFAULT settings)
```
./myMonitoringTool
 ```
- You can use multiple command line arguments to change how the program runs:
``` 
./myMonitoringTool  [samples [tdelay]] [--memory] [--cpu] [--cores] [--samples=N] [--tdelay=T]
```

More information about: [Commands Breakdown](#commands)


## Insights into Mechanics
This program is written in C language and it displays **UNIX** system information in real time using graphs.
With this program, you can view the following system information:
- Memory Usage (in GB)
- CPU Utilization (in %)
- Number of CPU cores and max CPU frequency (in GHz)

This program reads data from the following UNIX files:
- /proc/meminfo
- /proc/stat
- /proc/cpuinfo
- /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq

The graphs in the terminal are all updated using **ANSI** escape sequences.
You need to have a terminal that supports **ANSI** escape sequences, and access to a **UNIX** system, and **GCC** (or any alternative C compiler).

This README file is intended to give a High-Level Overview of my system_monitory tool, how to compile it, and how its various options allows the user to monitor system statistics.
