
Document For EE450 Final Project

============================================


(a)Written by : Honglei Wu
(b)

(c)What I have done in this project :
	Learn network system call getaddrinfo(), socket(), bind(), connect(), listen(), accept(), send(), recv(), sendto() and recvfrom(). Using these function to simulate a patient-healthcenter-doctor scenario. 

(d)What the code file are and what each of them does :
	healthcenterserver.c : implement the things server should do.
	patient1.c : implement Patient1.
	patient2.c : implement Patient2.
	doctor.c : implement doctor1 and doctor2 by multi-thread

(e)What the TA should do to run your programs :
	First, in current work directory, you can find a makefile file is in it. Just type "make" to compile to get 4 execute files.
	Second,  run server, doctor, then run patient1, patient2 by type "./server","./doctor","./patient1","./patient2".

(f)The format of all the messages exchanged :
	All messages are exchanged in string(char []) format.

(g)I use a another txt called "reservedslot.txt" to store the reserved slot information. Please DO NOT delete this reservedslot.txt.

(h)All code is writted by myself.

(i)I use fork() in healthcenterserver.c and multi_thread in doctor, also I use makefile.
