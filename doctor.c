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

#define MAXDATASIZE 100 // max number of bytes we can get at once 

typedef struct priceinfo{
char insurance[20];
char price[20];
}priceinfo_t;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
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
  if(sa->sa_family==AF_INET)
    {
      return &(((struct sockaddr_in*)sa)->sin_port);
    }
  return &(((struct sockaddr_in6*)sa)->sin6_port);
}

void printownportnum(int sockfd)
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
    printf("portnumber %d ", ntohs(sin.sin_port));
  for(m=0;hent->h_addr_list[m];m++)
    {
      printf("and IP address %s.\n", inet_ntoa(*(struct in_addr*)(hent->h_addr_list[m])));
    }
}

void *doc1()
{
  int sockfd, numbytes,i=0;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	socklen_t addr_len;
	struct sockaddr_storage their_addr;
        
        /*load the user logininfo*/
        char infobuf[50];
        FILE *fp;
        fp=fopen("doc1.txt","r");
        char *start_ptr;
        char *tab_ptr;
        priceinfo_t *priceinfop[3];
        priceinfop[0]=(priceinfo_t *)malloc(sizeof(priceinfo_t));
        priceinfop[1]=(priceinfo_t *)malloc(sizeof(priceinfo_t));
        priceinfop[2]=(priceinfo_t *)malloc(sizeof(priceinfo_t));

    while((fgets(infobuf,sizeof(infobuf),fp))!=NULL)
    {
        /*divide the username and password*/
        start_ptr=infobuf;
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
        strcpy(priceinfop[i]->insurance,start_ptr);
        
        start_ptr=tab_ptr;
        tab_ptr=strchr(start_ptr,'\n');
        if(tab_ptr !=NULL)
          {
            *tab_ptr='\0';
          }
        strcpy(priceinfop[i]->price,start_ptr);
        /*printf("%s\n",priceinfop[i]->insurance);
        printf("%s\n",priceinfop[i]->price);*/
        i++;
    }
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags=AI_PASSIVE; //use my IP

	if ((rv = getaddrinfo(NULL, "41097", &hints, &servinfo)) != 0) {
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

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		exit(1);
	}

	//inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
		//	s, sizeof s);
	//printf("client: connecting to %s\n", s);
	pthread_mutex_lock(&mutex);
	printf("Phase 3: Doctor 1 has a static UDP ");
	printownportnum(sockfd);
	pthread_mutex_unlock(&mutex);

	freeaddrinfo(servinfo); // all done with this structure
	while(1)
	{
	addr_len=sizeof(their_addr);
	recvfrom(sockfd,buf,MAXDATASIZE-1,0,(struct sockaddr *)&their_addr, &addr_len);
	char bufforip[200];
	char bufforport[200];
	inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),bufforip,sizeof bufforip);
	printf("Phase 3: Doctor 1 receives the request from the patient with port number %d and the insurance plan %s.\n", *((unsigned short int *)(get_in_port((struct sockaddr *)&their_addr))),buf);

	for(i=0;i<3;i++)
	{
		/**I think both the buf and priceinfop has /n,so it seems alright***/
	 if(strcmp(buf,priceinfop[i]->insurance)==0)
	  {
	    int numbers;
	    numbers= sendto(sockfd, priceinfop[i]->price, 10, 0, (struct sockaddr *)&their_addr, addr_len);
	    printf("Phase 3: Doctor 1 has sent estimated price %s$ to patient with port number %d.\n",priceinfop[i]->price,*((unsigned short int *)(get_in_port((struct sockaddr *)&their_addr))));
 }

	}
    } 

	close(sockfd);
  return (0);
}

void *doc2()
{
   int sockfd, numbytes,i=0;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	socklen_t addr_len;
	struct sockaddr_storage their_addr;
        
        /*load the user logininfo*/
        char infobuf[50];
        FILE *fp;
        fp=fopen("doc2.txt","r");
        char *start_ptr;
        char *tab_ptr;
        priceinfo_t *priceinfop[3];
        priceinfop[0]=(priceinfo_t *)malloc(sizeof(priceinfo_t));
        priceinfop[1]=(priceinfo_t *)malloc(sizeof(priceinfo_t));
        priceinfop[2]=(priceinfo_t *)malloc(sizeof(priceinfo_t));

    while((fgets(infobuf,sizeof(infobuf),fp))!=NULL)
    {
        /*divide the username and password*/
        start_ptr=infobuf;
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
        strcpy(priceinfop[i]->insurance,start_ptr);
        
        start_ptr=tab_ptr;
        tab_ptr=strchr(start_ptr,'\n');
        if(tab_ptr !=NULL)
          {
            *tab_ptr='\0';
          }
        strcpy(priceinfop[i]->price,start_ptr);
        /*printf("%s\n",priceinfop[i]->insurance);
        printf("%s\n",priceinfop[i]->price);*/
        i++;
    }
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags=AI_PASSIVE; //use my IP

	if ((rv = getaddrinfo(NULL, "42097", &hints, &servinfo)) != 0) {
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

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		exit(1);
	}

	//inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
		//	s, sizeof s);
	//printf("client: connecting to %s\n", s);
	pthread_mutex_lock(&mutex);

	printf("Phase 3: Doctor 2 has a static UDP ");
	printownportnum(sockfd);
	pthread_mutex_unlock(&mutex);

	freeaddrinfo(servinfo); // all done with this structure
	while(1)
	{
	addr_len=sizeof(their_addr);
	recvfrom(sockfd,buf,MAXDATASIZE-1,0,(struct sockaddr *)&their_addr, &addr_len);
	char bufforip[200];
	char bufforport[200];
	inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),bufforip,sizeof bufforip);
	printf("Phase 3: Doctor 2 receives the request from the patient with port number %d and the insurance plan %s.\n", *((unsigned short int *)(get_in_port((struct sockaddr *)&their_addr))),buf);

	for(i=0;i<3;i++)
	{
		/**I think both the buf and priceinfop has /n,so it seems alright***/
	 if(strcmp(buf,priceinfop[i]->insurance)==0)
	  {
	    int numbers;
	    numbers= sendto(sockfd, priceinfop[i]->price, 10, 0, (struct sockaddr *)&their_addr, addr_len);
	    printf("Phase 3: Doctor 2 has sent estimated price %s$ to patient with port number %d.\n",priceinfop[i]->price,*((unsigned short int *)(get_in_port((struct sockaddr *)&their_addr))));
 }

	}
    } 

	close(sockfd);
  return (0);
}

int main(void)
{
        
        pthread_t thr[2];
        if(pthread_create(&thr[0],0,doc1,0))
          {
            fprintf(stderr,"Error: pthread_create error!\n");
            exit(1);
          }
        if(pthread_create(&thr[1],0,doc2,0))
          {
            fprintf(stderr,"Error: pthread_create error!\n");
            exit(1);
          }
        pthread_join(thr[0],0);
        pthread_join(thr[1],0);

	return 0;
}

