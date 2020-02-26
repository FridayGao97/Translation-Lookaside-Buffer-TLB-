Description:
In this project, I will implement a simulator to study the TLB behavior of a running process by examining the output of valgrind for
different programs and different sizes of instances. In addition, two eviction policies represent two way of process the TLB.
The output results have been recored and visualized by execl. The report shows our understanding of the TLB performance corresponding to various parameters.

-----------------------------------------------------------------------------------------------------------------------
Instruction to run the code:
1. Go to the correct directory of the program.
2. Use make to compile the code to produce the executable for all programs.
3. Run the program by valgrind --tool=lackey --trace-mem=yes ./heapsort #instace_size 2>&1 | ./valtlb379 -i ....
4. Select the parameters that user wants to test.
5. The sorting exectuable are used for generating the input for simulation
6. Result will contains the number of memory references handled, the number of TLB misses, and the number of TLB hits.

-----------------------------------------------------------------------------------------------------------------------
Testing:
We have tested the code with different sample programs and instances. Also, the code was tested both (and running) on the
VM as well as on the physical host labs.