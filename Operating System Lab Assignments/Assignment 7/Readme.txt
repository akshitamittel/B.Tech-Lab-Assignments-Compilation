Readme:

Open a terminal:
Get into the directory of the code
Gain root access
type: sudo -s
type: [Your password]

compile longrun.c:
	type: gcc cs13b1040-longrun.c -o longrun
compile cs13b1040-sched.c
	type: gcc cs13b1040-sched.c -o cs13b1040-sched
run cs13b1040-sched.c
	type: ./cs13b1040-sched <input text file>

Here <input text file> is one of the following:
1. test1.txt
2. test2.txt
3. test3.txt
4. test4.txt
5. test5.txt
6. test6.txt

The format of the input files is as follows:
a
bi ci

where a = the number of processes
bi and ci are the ith policy and priority

bi:
1 == SCHED_FIFO
2 == SCHED_RR
3 == SCHED_OTHER

ci
1 == HIGHEST
2 == LOWEST
3 == static 0
