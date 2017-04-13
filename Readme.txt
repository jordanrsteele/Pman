PMan.c version 1.0 02/10/2016

PMan.c is a top level program manager capable of executing and controlling background processes simultaneously. 

Compile/Run:
	gcc PMan.c -o PMan
	./PMan 

-----------------
How To Run PMan.c 
-----------------



Step1:		Enter: make 
			
			-A Make file has been provided, this should compile all programs needed for PMan.c to run

			
Step2:		Enter: ./PMan
			
			-This will run PMan


Step3:		Enter: bg cat (some.txt file)

			bg is a function and cat is an example command, following are the arguments

			
Step4:		Enter: bglist

			-Will list functions running


step5:		Enter: pstat (some PID)

			-Will give stats on pid


step6:		Enter: bgkill (some pid)

			-Will kill a process

			



Functionality:

	bg - bg will run a compiled c file which can be referenced with its 	 associated PID. 
					Ex->	PMan: >bg ./inf hello 10
					out->	The pid of the child process is: 13628

	bgkill/bgstop/bgstart (pid) - bgkill will kill a background process using the cataloged PID
					Ex->	PMan: >bgkill 13628
					out->	Process 13628 has been terminated

	bglist - bglist will print out PIDs and commands from all running background processes
					Ex->	PMan: >bglist
					out->	14643:   ./inf
							14649:   ./inf
							14655:   ./inf
							Total Background Jobs: 3

	pstat (pid) - pstat returns a data printout of information on the given PID. 
					
					Ex->	PMan: >pstat 15751
					out->	/proc/15751/stat
							Command: (inf)
							State:   S
							utime:   0
							rss:     4292608
							voluntary_ctxt_switches:	4







