Write a Memory Management simulator program. Upload your program to Github and submit your link.

Assume your computer has a memory size of 16GB and it is divided into 100 pages. Hence each page is a size of 160 MB. Your program will create an array of size 100 elements and you maintain the processor numberer in the array. You will then call the userMemoryAllocation method to create processes until all the elements of the array are occupied by process number. Before the program is terminated you will report the jobs/processes status in the memory in the following format. The process allocation starts from memory address 2000 upwards.

Summary Report Format Example:

Process Id      Starting Memory Address      Size of the Process MB         Unused Space MG 

      4                         2000                                           2500                                           60

 

Method:  userMemoryAllocation

By using random number generator create a loop until you run of space.

      -Generate a number between 1 to 30 to determine the size of a process.

-Loop index value is the process number and the number generated is used to determine the number of pages that will be allocated. 
Each number represents 80 MB .

      - The size of the process is computed by number of page x 80

Example: Random Number= 25(number of pages) Process size= 25x80   process size = 2000 MB  

Find the number of Memory pages required to store the process in the memory.

      2000/160 = 12.5 (You need to round it up to an integer number). Hence you will need 13 memory     
      pages to keep the process in the memory.

Find the unused memory space.

        Total of 13 pages = 13x160 = 2080 bytes

        Unused memory space= 2080-2000 = 80 bytes.

The next process starting address = 2000+2080 = 4080