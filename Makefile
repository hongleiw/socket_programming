honglei: patient1 patient2 doctor server 
patient1: patient1.c
	gcc -o patient1 patient1.c -lsocket -lnsl -lresolv
patient2: patient2.c
	gcc -o patient2 patient2.c -lsocket -lnsl -lresolv
doctor: doctor.c
	gcc -o doctor doctor.c -lsocket -lnsl -lresolv -lpthread
server: healthcenterserver.c
	gcc -o server healthcenterserver.c -lsocket -lnsl -lresolv
clean:
	rm -f *.o patient1 patient2 doctor server
