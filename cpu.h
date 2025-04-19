#ifndef CPU_H
#define CPU_H

void dataCPU(long* total, long* idle2);
int calculateCPU(long* prev_total, long* prev_idle, float* cpu_util);
void plotCPU(int row_main, int row, int col_main, float cpu_util);
void conceiveGraCPU(int row, int row_main, int samplesNow, float cpu_util);
#endif