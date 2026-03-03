#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>   
#include <sys/wait.h>  

#define TOTAL_PAGES 100
#define PAGE_SIZE_MB 160
#define UNIT_MB 80
#define MIN_UNITS 1
#define MAX_UNITS 30
#define START_ADDRESS 2000

static int ceil_div(int a, int b) {return (a + b - 1) / b;}

static int read_full(int fd, void *buf, size_t n) 
{
    size_t got = 0;
    char *p = (char*)buf;
    while (got < n) {
        ssize_t r = read(fd, p + got, n - got);
        if (r <= 0) return -1;
        got += (size_t)r;
    }
    return 0;
}

static int write_full(int fd, const void *buf, size_t n) 
{
    size_t sent = 0;
    const char *p = (const char*)buf;
    while (sent < n) 
    {
        ssize_t w = write(fd, p + sent, n - sent);
        if (w <= 0) return -1;
        sent += (size_t)w;
    }
    return 0;
}

static int userMemoryAllocation(int memory[TOTAL_PAGES], int procIdArr[TOTAL_PAGES], int startAddrArr[TOTAL_PAGES],
                                int sizeMBArr[TOTAL_PAGES], int unusedMBArr[TOTAL_PAGES]) 
{
    int nextFreePage = 0;
    int nextStartAddr = START_ADDRESS;
    int processId = 1;
    int count = 0;

    while (nextFreePage < TOTAL_PAGES) 
    {
        int remainingPages = TOTAL_PAGES - nextFreePage;

        // Calculate max units that can fit in remaining pages
        int maxUnitsThatFit = 2 * remainingPages - 1;
        if (maxUnitsThatFit > MAX_UNITS) maxUnitsThatFit = MAX_UNITS;
        if (maxUnitsThatFit < MIN_UNITS) maxUnitsThatFit = MIN_UNITS; // safety check; should never happen

        int pipefd[2];
        if (pipe(pipefd) != 0) 
        {
            perror("pipe");
            exit(1);
        }

        pid_t pid = fork();
        if (pid < 0) 
        {
            perror("fork");
            exit(1);
        }

        if (pid == 0) 
        {
            close(pipefd[0]);

            unsigned seed = (unsigned)(time(NULL) ^ (getpid() << 16));
            srand(seed);

            int units = (rand() % maxUnitsThatFit) + MIN_UNITS; // random units between 1 and maxUnitsThatFit
            if (write_full(pipefd[1], &units, sizeof(units)) != 0) {
                // In child, if write fails, just exit; parent will handle it
                _exit(2);
            }

            close(pipefd[1]);
            _exit(0);
        }

        close(pipefd[1]);

        int units = 0;
        if (read_full(pipefd[0], &units, sizeof(units)) != 0) 
        {
            perror("read");
            exit(1);
        }
        close(pipefd[0]);

        int status = 0;
        waitpid(pid, &status, 0);

        // Compute process + allocation
        int processSizeMB = units * UNIT_MB;
        int pagesNeeded = ceil_div(processSizeMB, PAGE_SIZE_MB);
        int allocatedMB = pagesNeeded * PAGE_SIZE_MB;
        int unusedMB = allocatedMB - processSizeMB;

        // Allocate pages in memory array
        for (int i = 0; i < pagesNeeded; i++) {
            memory[nextFreePage + i] = processId;
        }

        // Record info (no structs; parallel arrays)
        procIdArr[count]    = processId;
        startAddrArr[count] = nextStartAddr;
        sizeMBArr[count]    = processSizeMB;
        unusedMBArr[count]  = unusedMB;
        count++;

        // Next process
        nextFreePage += pagesNeeded;
        nextStartAddr += allocatedMB;
        processId++;
    }

    return count;
}

static void printSummaryReport(const int procIdArr[], const int startAddrArr[], const int sizeMBArr[], const int unusedMBArr[], int count) 
{
    printf("\nSummary Report\n");
    printf("Process Id\tStarting Memory Address\tSize of the Process (MB)\tUnused Space (MB)\n");
    printf("----------\t-----------------------\t------------------------\t----------------\n");

    for (int i = 0; i < count; i++)  {printf("%10d\t%23d\t%24d\t%16d\n",procIdArr[i], startAddrArr[i], sizeMBArr[i], unusedMBArr[i]);}

    printf("\nTotal processes created: %d\n", count);
    printf("Memory filled: %d pages, %d MB/page\n", TOTAL_PAGES, PAGE_SIZE_MB);
}

int main(void) 
{
    int memory[TOTAL_PAGES] = {0};

    int procIdArr[TOTAL_PAGES] = {0};
    int startAddrArr[TOTAL_PAGES] = {0};
    int sizeMBArr[TOTAL_PAGES] = {0};
    int unusedMBArr[TOTAL_PAGES] = {0};

    int count = userMemoryAllocation(memory, procIdArr, startAddrArr, sizeMBArr, unusedMBArr);
    printSummaryReport(procIdArr, startAddrArr, sizeMBArr, unusedMBArr, count);

    return 0;
}