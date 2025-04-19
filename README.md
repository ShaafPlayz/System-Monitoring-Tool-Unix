# System Monitoring Tool with Concurrency

## *Supported Commands*
The program accepts the following command-line arguments:

***By Default, if  no arguemnts are provided, our program will run ```--umem```, ```--ucpu```, ```--ucores``` and will display all currently running processes.***
You have access to the following command line arguments (Flags) that the program accepts:
- ```--ucpu``` (Displays CPU Utilization Graph in %)
- ```--umem``` (Displays Memory Usage Graph in GB)
- ```--ucores``` (Returns Number of CPU Cores along with Max CPU Frequency)
- ```--nsam=N``` (If used the value **N** will indicate how many statistics are collected)
- ```--ninterval=T``` (If used the value **T** will indicate how frequently to collect satatistics)

In addition to above Command Line Graphs, we can use the following keyboard shortcuts to control the program runtime:
- ```CTRL + C``` This shortcut would suspend and prompt the user if they would like to EXIT or RESUME the program.
- ```CTRL + Z``` This shortcut is ignored by the program.

## *Test Cases*

The program will run as many recognized arguments: 

- ```./program.out 50 600000 --ucpu --ninterval=100```

The program sees "--ucpu" command above, this indicates that the program will now only display the CPU graph. Observe that "50" is read as the number of samples, "600000" is read as ninterval in microseconds, and the extra "--ninterval=100" command is ignored. 

- ```./program.out --umem efwfwefewf```
- ```./main.out efwfwefewf```

In the arugments provided above, the program will ignore the gibrish and run the the recognized commands within them (eg: ```--umem```).

## *Instructions on Compiling*
Using the makefile, we can compile the code using ```make``` command in the terminal.\
We can also use ```make run``` to run our program, but this would cause our signals to not work as expected.\
We can also use ```make clean``` to delete all files produced by make including the executable.\
If we instead would like to compile the program without makefile, we can use the following command: \
```C
gcc -std=gnu99 -o program.out main.c memory.c cores.c cpu.c
```

## *Introduction*
This program is written in C language and it displays **UNIX** system information in real time using graphs.
With this program, you can view the following system information:
- Memory Usage (in GB)
- CPU Utilization (in %)
- Number of CPU cores and max CPU frequency (in GHz)

All three functions above run in concurrency to the main process.


## *Program FlowChart*
![alt text](flowchara3.png)



