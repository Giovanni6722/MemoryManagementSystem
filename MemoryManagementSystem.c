//gcc -std=c11 -Wall -Wextra -O2 MemoryManagementSystem.c -o MemoryManagementSystem.exe
//./MemoryManagementSystem.exe
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_PAGES 100
#define PAGE_SIZE_MB 160
#define UNIT_MB 80
#define MIN_UNITS 1
#define MAX_UNITS 30
#define START_ADDRESS 2000

typedef struct 
{
    int processId;
    int startingAddress;
    int processSizeMB;
    int unusedMB;
}   ProcessRecord;

static int ceil_div(int a, int b) {return (a + b - 1) / b;}

/*
- Randomly generates processes until memory (100 pages) is full.
- Each random number (units) represents 80MB, process size = units * 80MB.
- Pages needed: ceil(processSize / 160MB).
- Allocated MB: pagesNeeded * 160MB.
- Unused MB: allocatedMB - processSizeMB.
- Starting address begins at 2000 and increases by allocatedMB each process.
*/
static int userMemoryAllocation(int memory[TOTAL_PAGES], ProcessRecord records[TOTAL_PAGES]) 
{
    int nextFreePage = 0;
    int nextStartAddress = START_ADDRESS;
    int processId = 1;
    int recordCount = 0;

    while (nextFreePage < TOTAL_PAGES) 
    {
        int remainingPages = TOTAL_PAGES - nextFreePage;

        /*
          pick units so it fits in remaining pages
          pagesNeeded = ceil((units*80)/160) = ceil(units/2) = (units+1)/2 (integer math)
          Condition: (units+1)/2 <= remainingPages  => units <= 2*remainingPages - 1
        */
        int maxUnitsThatFit = 2 * remainingPages - 1;
        if (maxUnitsThatFit > MAX_UNITS) maxUnitsThatFit = MAX_UNITS;

        //safety (should never happen because remainingPages >= 1 => maxUnitsThatFit >= 1)
        if (maxUnitsThatFit < MIN_UNITS) break;

        int units = (rand() % maxUnitsThatFit) + MIN_UNITS;   // random in [1 .. maxUnitsThatFit]
        int processSizeMB = units * UNIT_MB;

        int pagesNeeded = ceil_div(processSizeMB, PAGE_SIZE_MB);
        int allocatedMB = pagesNeeded * PAGE_SIZE_MB;
        int unusedMB = allocatedMB - processSizeMB;

        //fill memory pages with this processId
        for (int i = 0; i < pagesNeeded; i++) {memory[nextFreePage + i] = processId;}

        //record for summary report
        records[recordCount].processId = processId;
        records[recordCount].startingAddress = nextStartAddress;
        records[recordCount].processSizeMB = processSizeMB;
        records[recordCount].unusedMB = unusedMB;
        recordCount++;

        //update for next process
        nextFreePage += pagesNeeded;
        nextStartAddress += allocatedMB;
        processId++;
    }

    return recordCount;
}

static void printSummaryReport(const ProcessRecord records[], int count) 
{
    printf("\nSummary Report\n");
    printf("Process Id\tStarting Memory Address\tSize of the Process (MB)\tUnused Space (MB)\n");
    printf("----------\t-----------------------\t------------------------\t----------------\n");

    for (int i = 0; i < count; i++) 
    {
        printf("%10d\t%23d\t%24d\t%16d\n",
               records[i].processId,
               records[i].startingAddress,
               records[i].processSizeMB,
               records[i].unusedMB);
    }
    printf("\nTotal processes created: %d\n", count);
}

static void printMemoryMap(const int memory[TOTAL_PAGES]) 
{
    printf("\nMemory Page Map (pageIndex:processId)\n");
    for (int i = 0; i < TOTAL_PAGES; i++) 
    {
        printf("%2d:%d  ", i, memory[i]);
        if ((i + 1) % 10 == 0) printf("\n");
    }
}

int main(void) 
{
    int memory[TOTAL_PAGES] = {0};
    ProcessRecord records[TOTAL_PAGES];

    srand((unsigned)time(NULL)); // random seed

    int count = userMemoryAllocation(memory, records);
    printSummaryReport(records, count);

    return 0;
}