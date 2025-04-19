#ifndef MEMORY_H
#define MEMORY_H

void dataMem(int *row, double *mem_util_used, double *display_ram);
void plotMem(int row_main, int col_main, int row, double mem_util_used);
void conceiveGraMem(int samplesNow, int row, double mem_util_used, double display_ram);

#endif