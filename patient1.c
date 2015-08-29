#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "21097" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

typedef struct userinfo{
char name[20];
char password[20];
}patientinfo;

int sockfd;

typedef struct availabilitiesforcommunication{
   char index[5];
   char day[5];
   char atime[5];
}avabfc_t; 

avabfc_t *buffc[10];
char indexchoose[20];
char port[6];
//int sockfd;


void patientchoose(void)
{
    printf("Please input preferred time slot:\n");
    strcpy(indexchoose,"selection");
    char ch;

    scanf("%c",&indexchoose[10]);
    while((ch=getchar())!='\n'&&ch!=EOF);
    indexchoose[11]='\0';
    indexchoose[9]=' ';
    


}
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
/*
char *receivedocinfo(void)
{
    char *bufdoc;
    bufdoc=(char *)malloc(20);
    recv(sockfd, bufdoc, sizeof(bufdoc), 0);
    return bufdoc;
}
*/
void getport(char *arg)
{
    int i;
    for(i=0;i<5;i++)
        {
            port[i]=arg[i+4];
        }
        port[5]='\0';
    
}

void printownportnum()
{
  struct sockaddr_in sin;
  socklen_t len=sizeof(sin);
  char hostname[128];
  struct hostent *hent;
  int m;
  gethostname(hostname,sizeof(hostname));
  hent=gethostbyname(hostname);
  if(getsockname(sockfd,(struct sockaddr *)&sin, &len)==-1)
    perror("getsockname");
  else
    printf("port number %d ", ntohs(sin.sin_port));
  for(m=0;hent->h_addr_list[m];m++)
    {
      printf("and IP address %s.\n", inet_ntoa(*(struct in_addr*)(hent->h_addr_list[m])));
    }
}



int main(void)
{
    int i;
    int j;
       int numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    /*load the user info*/
        char loginbuf[50];
        FILE *fp;
        fp=fopen("patient1.txt","r");
        while((fgets(loginbuf,sizeof(loginbuf),fp))!=NULL)
        {
        if(strchr(loginbuf,'\n'))
          {
            *(strchr(loginbuf,'\n'))='\0';
          }
        }
        /*divide the username and password*/
        char *start_ptr;
        char *tab_ptr;
        patientinfo *patient1infop;
        patient1infop=(patientinfo *)malloc(sizeof(patientinfo));
        start_ptr=loginbuf;
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
        fclose(fp);
        fp=fopen("patient1insurance.txt","r");
        fgets(loginbuf,sizeof(loginbuf),fp);
        loginbuf[10]='\0';
        fclose(fp);
        

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        exit(1);
    }
    printf("Phase 1: Patent 1 has TCP ");
    printownportnum();
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
        
        if (send(sockfd, patient1infop, sizeof(patientinfo), 0) == -1){
                perror("send");
                                exit(1);
        }
	printf("Phase 1: Authentication request from Patient 1 with username %s and password %s has been sent to the Health Center Server.\n",patient1infop->name, patient1infop->password);
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("Phase 1: Patient 1 anthentication result:%s\n",buf);
    printf("Phase 1: End of Phase 1 for patient 1\n");
    if(strcmp(buf,"failure!")==0)
    {

        printf("cannot login: information incorrect!");
        sleep(2);
        exit(1);
    }
    if (send(sockfd, "availability", sizeof("availability"), 0) == -1){
                perror("send");
                                exit(1);
        }

    for(i=0;i<6;i++)
    {
        buffc[i]=(avabfc_t *)malloc(sizeof(avabfc_t));
    }
    char num[3];
     recv(sockfd, num, 5, 0);
     j=atoi(num);
     char unixbuf[50];
     /*
     for(i=0;i<6;i++)
       {
     unixbuf[i]=(char *)malloc(sizeof(50));
       }
     */
    for(i=0;i<j;i++)
    {
    recv(sockfd, unixbuf, 50, 0);
    (buffc[i]->index)[0]=unixbuf[0];
    (buffc[i]->index)[1]='\0';
    (buffc[i]->day)[0]=unixbuf[1];
    (buffc[i]->day)[1]=unixbuf[2];
    (buffc[i]->day)[2]=unixbuf[3];
    (buffc[i]->day)[3]='\0';
    (buffc[i]->atime)[0]=unixbuf[4];
    (buffc[i]->atime)[1]=unixbuf[5];
    (buffc[i]->atime)[2]=unixbuf[6];
    (buffc[i]->atime)[3]=unixbuf[7];
    (buffc[i]->atime)[4]='\0';
    }
    printf("Phase 2: The following appointments are available for Patient 1:\n");
    for(i=0;i<j;i++)
    {
    printf("index:%s,day:%s,time:%s.\n",buffc[i]->index,buffc[i]->day,buffc[i]->atime);
    }
    patientchoose();
    while(!(indexchoose[10]=='1'||indexchoose[10]=='2'||indexchoose[10]=='3'||indexchoose[10]=='4'||indexchoose[10]=='5'||indexchoose[10]=='6'))
     {

    printf("usage:input the index of the slot displayed you like, like '1' or '2'\n");
    patientchoose();
     }
       send(sockfd, indexchoose, 20, 0);

    char bufdoc[20];
    //bufdoc=(char *)malloc(20);
    recv(sockfd, bufdoc, sizeof(bufdoc), 0);
    if(strcmp(bufdoc,"notavailable")==0)
    {
      printf("Phase 2: The requested appointment from Patient 1 is not available. Exiting...\n");
        close(sockfd);
	sleep(2);
           return 0;
    }
    else
    {
      getport(bufdoc);
      printf("Phase 2: The requested appointment is availble and reserved to patient1. The assigned doctor port is %s\n", port);
        close(sockfd);
    }
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo("localhost", port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }
    printf("Phase 3: Patient 1 has a dynamic UDP ");

 /********send insurance*********/
    if ((numbytes = sendto(sockfd, loginbuf, sizeof(loginbuf), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }
    printownportnum();
    bufdoc[4]='\0';
    printf("Phase 3: The cost estimation request from Patient 1 with insurance plan %s has been sent to the doctor with port number %s and name %s.\n",loginbuf, port,bufdoc);
/***************receive price******************/
    int numbers;

   numbers= recvfrom(sockfd,loginbuf,sizeof(loginbuf),0,NULL, NULL);
    printf("Phase 3: Patient 1 receives %s$ estimation cost from doctor with port number %s and name %s.\n", loginbuf,port,bufdoc);
    printf("Phase 3: End of Phase 3 for Patient 1\n");

    freeaddrinfo(servinfo);

    
    close(sockfd);
  return 0;
}
