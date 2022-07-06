#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT		8080
#define MAXLINE		64

struct date_time
{
	int second;
	int minute;
	int hour;
	int day;
	int month;
	int year;
};

int		valid_date(void);
int 	valid_time(void);
void 	get_date_and_time(char[]);
struct	sockaddr_in	servaddr;
int		sockfd;
char	*messageToServer	= NULL;
char	messageFromServer[MAXLINE];
int 	length, bytesNumber;
int 	newProcessID;
struct 	date_time new_date;

int main(int argc, char* argv[]) 
{
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;
	
	while(1)
	{
		get_date_and_time(argv[2]);
		printf("\nMessage sent to the server: %s\n", messageToServer);
		
	  	sendto(sockfd, (const char *)messageToServer, strlen(messageToServer), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
	  	
		bytesNumber = recvfrom(sockfd, (char *)messageFromServer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &length);
		
		messageFromServer[bytesNumber] = '\0';
		printf("Message received from server : %s\n", messageFromServer);
		exit(0);
		close(sockfd);
	}
	return 0;
}


void get_date_and_time(char commandParameter[])
{
	printf("\nEnter the date with format(dd/mm/yyyy): ");
	scanf("%d/%d/%d", &new_date.day, &new_date.month, &new_date.year);
	if(valid_date())
	{
		printf("\nEnter the time with format(hh:mm:ss): ");
		scanf("%d:%d:%d", &new_date.hour, &new_date.minute, &new_date.second);
		if(valid_time())
		{
			messageToServer = (char*)malloc(sizeof(char) * 22);
			//DD/MM/YYYY HH/MM/SS -Xn
			 unsigned int i = 0;
			*(messageToServer + i++) = new_date.day/10 + '0';
			*(messageToServer + i++) = new_date.day%10 + '0';
			*(messageToServer + i++) = '/';
			*(messageToServer + i++) = new_date.month/10 + '0';
			*(messageToServer + i++) = new_date.month%10 + '0';
			*(messageToServer + i++) = '/';
			*(messageToServer + i++) = new_date.year/1000 + '0';
			*(messageToServer + i++) = (new_date.year/100)%10 + '0';
			*(messageToServer + i++) = (new_date.year/10)%10 + '0';
			*(messageToServer + i++) = new_date.year%10 + '0';
			*(messageToServer + i++) = ' ';
			*(messageToServer + i++) = new_date.hour/10 + '0';
			*(messageToServer + i++) = new_date.hour%10 + '0';
			*(messageToServer + i++) = ':';
			*(messageToServer + i++) = new_date.minute/10 + '0';
			*(messageToServer + i++) = new_date.minute%10 + '0';
			*(messageToServer + i++) = ':';
			*(messageToServer + i++) = new_date.second/10 + '0';
			*(messageToServer + i++) = new_date.second%10 + '0';
			*(messageToServer + i++) = ' ';
			*(messageToServer + i++) = commandParameter[0];
			*(messageToServer + i++) = commandParameter[1];
			*(messageToServer + i++) = '\0';
		}
		else
		{
			printf("Error: time is not valid.\n");
			exit(0);
		}
	}
	else
	{
		printf("Error: date is not valid.\n");
		exit(0);
	}
}

int valid_date()
{
	if(new_date.year>=1970 && new_date.year<=4000)
	{
		if(new_date.month>=1 && new_date.month<=12)
		{
			if((new_date.day>=1 && new_date.day<=31) && (new_date.month==1 || new_date.month==3 || new_date.month==5 || new_date.month==7 || new_date.month==8 || new_date.month==10 || new_date.month==12)) return 1;
			else if((new_date.day>=1 && new_date.day<=30) && (new_date.month==4 || new_date.month==6 || new_date.month==9 || new_date.month==11)) return 1;
			else if((new_date.day>=1 && new_date.day<=28) && (new_date.month==2)) return 1;
			else if(new_date.day==29 && new_date.month==2 && (new_date.year%400==0 ||(new_date.year%4==0 && new_date.year%100!=0))) return 1;
			else printf("The day is not valid.\n");
		}
		else printf("The month is not valid.\n");
	}
	else printf("The year is not valid.\n");
	return 0;
}

int valid_time()
{
	if(new_date.second < 0 || new_date.second > 59)
	{
		printf("The second is not valid.\n");
		return 0;
	}
	if(new_date.minute < 0 || new_date.minute > 59)
	{ 
		printf("The minute is not valid.\n");
		return 0;
	}
	if(new_date.hour < 0 || new_date.hour > 23)
	{ 
		printf("The hour is not valid.\n");
		return 0;
	}
	return 1;
}
