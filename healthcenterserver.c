#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "21097"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

/*struct for available list*/
typedef struct availabilities{
   char index[5];
   char day[5];
   char atime[5];
   char doc[5];
   char portnum[10];
   int iffree;  //if free 1, if not 0;
}avab_t; 

typedef struct availabilitiesforcommunication{
   char index[5];
   char day[5];
   char atime[5];
}avabfc_t; 

typedef struct userinfo{
          char name[20];
          char password[20];
        }patientinfo;

avab_t *avabp[6];
avabfc_t *avabfcp[6];
char indexchoose[20];
FILE *reservefp;
patientinfo *patient1infop;
patientinfo *patient2infop;
int new_fd;

void Readavab(void)
{

  char buf[50];
  FILE *fp;
  fp=fopen("availabilities.txt","r");
  char *start_ptr;
    char *tab_ptr;
    int i=0;
    
    avabp[0]=(avab_t *)malloc(sizeof(avab_t));
    avabp[1]=(avab_t *)malloc(sizeof(avab_t));
    avabp[2]=(avab_t *)malloc(sizeof(avab_t));
    avabp[3]=(avab_t *)malloc(sizeof(avab_t));
    avabp[4]=(avab_t *)malloc(sizeof(avab_t));
    avabp[5]=(avab_t *)malloc(sizeof(avab_t));
  while((fgets(buf,sizeof(buf),fp))!=NULL)
  {
    
    start_ptr=buf;
    tab_ptr=strchr(start_ptr,' ');
        if(tab_ptr==NULL)
          {
            fprintf(stderr,"Error: the database format is error!");
          }
        if(tab_ptr !=NULL)
          {
            *tab_ptr='\0';
            tab_ptr++;
          }
        strcpy(avabp[i]->index,start_ptr);
        start_ptr=tab_ptr;
        tab_ptr=strchr(start_ptr,' ');
        if(tab_ptr==NULL)
          {
            fprintf(stderr,"Error:The use of   is wrong\n");
              exit(1);
          }
        if(tab_ptr !=NULL)
          {
            *tab_ptr='\0';
            tab_ptr++;
          }
        strcpy(avabp[i]->day,start_ptr);
        start_ptr=tab_ptr;
        tab_ptr=strchr(start_ptr,' ');
        if(tab_ptr==NULL)
          {
            fprintf(stderr,"Error:The use of \\t is wrong\n");
              exit(1);
          }
        if(tab_ptr !=NULL)
          {
            *tab_ptr='\0';
            tab_ptr++;
          }
        strcpy(avabp[i]->atime,start_ptr);
        start_ptr=tab_ptr;
        tab_ptr=strchr(start_ptr,' ');
        if(tab_ptr==NULL)
          {
            fprintf(stderr,"Error:The use of \\t is wrong\n");
              exit(1);
          }
        if(tab_ptr !=NULL)
          {
            *tab_ptr='\0';
            tab_ptr++;
          }
        strcpy(avabp[i]->doc,start_ptr);
        strcpy(avabp[i]->portnum,tab_ptr);
        avabp[i]->iffree=1;
        i++;
  }
  fclose(fp);
}  

void chstrufc(void)
{
  int i;
  //FILE *fp;
  //fp=fopen("reservedslot.txt","r");
  fseek(reservefp,0,SEEK_SET);
  char buf[10];
  avabfcp[0]=(avabfc_t *)malloc(sizeof(avabfc_t));
  avabfcp[1]=(avabfc_t *)malloc(sizeof(avabfc_t));
  avabfcp[2]=(avabfc_t *)malloc(sizeof(avabfc_t));
  avabfcp[3]=(avabfc_t *)malloc(sizeof(avabfc_t));
  avabfcp[4]=(avabfc_t *)malloc(sizeof(avabfc_t));
  avabfcp[5]=(avabfc_t *)malloc(sizeof(avabfc_t));
  for(i=0;i<6;i++)
  {
    strcpy(avabfcp[i]->index,avabp[i]->index);
    strcpy(avabfcp[i]->day,avabp[i]->day);
    strcpy(avabfcp[i]->atime,avabp[i]->atime);
  }

  while((fgets(buf,sizeof(buf),reservefp))!=NULL)
  {
    avabp[atoi(buf)-1]->iffree=0;
  }
  fseek(reservefp,0,SEEK_END);
}


void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *get_in_port(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_port);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_port);
}

int reserve(char arg)
{
  
  char buf[10];
  char bufdoc[20];
  fseek(reservefp,0,SEEK_SET);
  while((fgets(buf,sizeof(buf),reservefp))!=NULL)
  {
    if(buf[0]==arg)
    {
      send(new_fd, "notavailable", 20, 0);
      return 0;
    }
  }
  fseek(reservefp,0,SEEK_END);
  
  fprintf(reservefp, "%c\n", arg);


  strcpy(bufdoc,(avabp[atoi(&arg)-1]->doc));
  strcat(bufdoc,(avabp[atoi(&arg)-1]->portnum));
  send(new_fd, bufdoc, sizeof(bufdoc), 0);
  
  return 0;
}

void loaddatabase(void)
{
  /*load the user database*/
        char buf[50];
        char patient1lninfo[50];
        char patient2lninfo[50];
        int i=0;
        FILE *fp;
        fp=fopen("users.txt","r");
        while((fgets(buf,sizeof(buf),fp))!=NULL)
        {
        if(strchr(buf,'\n'))
          {
            *(strchr(buf,'\n'))='\0';
          }
        if(strlen(buf)>2)
          {
            if(i==0) 
                 strcpy(patient1lninfo,buf);
            if(i==1)
                  strcpy(patient2lninfo,buf);
          }
        i++;
        }
        /*divide the username and password*/
        char *start_ptr;
        char *tab_ptr;
        
        patient1infop=(patientinfo *)malloc(sizeof(patientinfo));
        patient2infop=(patientinfo *)malloc(sizeof(patientinfo));
        start_ptr=patient1lninfo;
        tab_ptr=strchr(start_ptr,' ');
        if(tab_ptr==NULL)
          {
            fprintf(stderr,"Error: the database format is error!");
          }
        if(tab_ptr !=NULL)
          {
            *tab_ptr='\0';
            tab_ptr++;
          }
        strcpy(patient1infop->name,start_ptr);
        strcpy(patient1infop->password,tab_ptr);
        
        start_ptr=patient2lninfo;
        tab_ptr=strchr(start_ptr,' ');
        if(tab_ptr==NULL)
          {
            fprintf(stderr,"Error: the database format is error!");
          }
        if(tab_ptr !=NULL)
          {
            *tab_ptr='\0';
            tab_ptr++;
          }
        strcpy(patient2infop->name,start_ptr);
        strcpy(patient2infop->password,tab_ptr);
        fclose(fp);

}

void getip(void)
{
  printf("Phase 1: The Health Center Server has port number ");
        printf(PORT);
  char hostname[128];
  struct hostent *hent;        
  int m;
  gethostname(hostname,sizeof(hostname));
  hent=gethostbyname(hostname);
  //printf("hostname:%s\n", hent->h_name);
  for(m=0;hent->h_addr_list[m];m++)
  {
    printf(" and IP address %s\n", inet_ntoa(*(struct in_addr*)(hent->h_addr_list[m])));
  }
}

int main(void)
{
	int sockfd;//, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	char port[INET6_ADDRSTRLEN];
	int rv;
  int j;
  int i;

  reservefp=fopen("reservedslot.txt","w+");

        loaddatabase();
        /*read the availabilities.txt*/
        Readavab();
        /*show server's IP address*/
        getip();

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
  
	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
                 /*in order for sure to reuse the port again*/
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}
  /*
  printf("Phase 1: The Health Center Server has port number ");
        printf(PORT);
        printf(" and IP address is %s\n", inet_ntoa(((struct sockaddr_in *)(p->ai_addr))->sin_addr));
        */
	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}
   
	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
		  //perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		inet_ntop(their_addr.ss_family,
			get_in_port((struct sockaddr *)&their_addr),
			port, sizeof port);
		

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
                        patientinfo *buf;
                        buf=(patientinfo *)malloc(sizeof(patientinfo));
                        int numbytes;
                        if ((numbytes = recv(new_fd, buf, sizeof(patientinfo), 0)) == -1) {
	                        perror("recv");
	                }

	                printf("Phase 1:The Health Center Server has received request from a patient with username '%s' and password '%s'.\n",buf->name,buf->password);
 
                        if(strcmp(buf->name,patient1infop->name)==0){
                             if(strcmp(buf->password,patient1infop->password)==0){

                             	if (send(new_fd, "success!", 13, 0) == -1){
					perror("send");
                                }
                                printf("Phase 1: The Health Center Server sends the response 'success' to patient with username %s.\n", buf->name);
                             }
                             else{
                             	if (send(new_fd, "failure!", 13, 0) == -1){
					perror("send");
                                }
                                printf("Phase 1: The Health Center Server sends the response 'failure' to patient with username %s.\n", buf->name);	
                             }
                        }
                        if(strcmp(buf->name,patient2infop->name)==0){
                             if(strcmp(buf->password,patient2infop->password)==0){
                             	if (send(new_fd, "success!", 13, 0) == -1){
					perror("send");
                                }
                                printf("Phase 1: The Health Center Server sends the response 'success' to patient with username %s.\n", buf->name);
                             }
                             else{
                             	if (send(new_fd, "failure!", 13, 0) == -1){
					perror("send");
                                }	
                                printf("Phase 1: The Health Center Server sends the response 'failure' to patient with username %s.\n", buf->name);
                             }
                        }
      char bufofavab[20];
      if (recv(new_fd, bufofavab, sizeof(bufofavab), 0) == -1) {
                          perror("recv");
                          exit(1);
                  }
      printf("Phase 2: The Health Center Server, receives a request for available time slots from patients with port number %d and IP address %s\n",  *((unsigned short int *)(get_in_port((struct sockaddr *)&their_addr))),s);
     /* printf("server: got connection from IP %s\n", s);
    printf("server: got connection from port %d\n", *((unsigned short int *)(get_in_port((struct sockaddr *)&their_addr))));
      */
      chstrufc();

      j=0;
      for(i=0;i<6;i++)
    { 
      if(avabp[i]->iffree==1)
      {
        j++;
        /*printf("index:%s,day:%s,time:%s.\n",avabfcp[i]->index,avabfcp[i]->day,avabfcp[i]->atime);*/

      }
    }
    printf("Phase 2: The Health Center Server sends available time slots to patient with username %s\n", buf->name);
      char num[3];
      sprintf(num,"%d",j);
    send(new_fd, num, sizeof(num), 0);
    char *unixbuf[6];
    for(i=0;i<6;i++)
      {
	unixbuf[i]=(char *)malloc(50);
	strcpy(unixbuf[i],avabfcp[i]->index);
	strcat(unixbuf[i],avabfcp[i]->day);
	strcat(unixbuf[i],avabfcp[i]->atime);
      }
    
      for(i=0;i<6;i++)
      {
        if(avabp[i]->iffree==1)
      {
        send(new_fd, unixbuf[i], 50, 0);
        
      }
        
      }

    recv(new_fd, indexchoose, 20, 0);
    printf("Phase 2: The Health Center Server receives a request for appointment %s from patient with portnumber %d and username %s\n", indexchoose, *((unsigned short int *)(get_in_port((struct sockaddr *)&their_addr))),buf->name);
    /*printf("patient choose index :%s\n", indexchoose);*/
    char doublecheckbuf[10];
    fseek(reservefp,0,SEEK_SET);
  while((fgets(doublecheckbuf,sizeof(doublecheckbuf),reservefp))!=NULL)
  {
    avabp[atoi(doublecheckbuf)-1]->iffree=0;
  }
  fseek(reservefp,0,SEEK_END);
  char *indexchoosenum;
  indexchoosenum=&indexchoose[10];
  if(avabp[atoi(indexchoosenum)-1]->iffree==1)
  {
    printf("phase 2: The Health Center Server confirms the following appointment %s to the patient with username %s\n", indexchoose, buf->name);
    reserve(indexchoose[10]); 
  }
  if(avabp[atoi(indexchoosenum)-1]->iffree==0)
  {
    printf("phase 2: The Health Center Server rejects the following appointment %s to the patient with username %s\n", indexchoose, buf->name);
  }
    
    /**phase2, stage2**/
    

			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}
  fclose(reservefp);
	return 0;
}

