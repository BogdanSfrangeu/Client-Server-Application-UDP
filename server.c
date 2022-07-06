#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT	 	8080
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

void new_date_and_time(void);

struct date_time start_date, end_date;
struct tm tm_start_date, tm_end_date;
time_t tm_start_time, tm_end_time;
int	   newProcessID;
char* 	messageToClient 	= NULL;
char 	messageFromClient[MAXLINE];
char	commandParameter;
int 	length, bytesNumber;
int 	sockfd;
unsigned long 	secondsDifference = 0;

int main() 
{
	struct sockaddr_in servaddr, cliaddr;
	
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
	
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
	while(1)
	{
		length = sizeof(cliaddr);
		bytesNumber = recvfrom(sockfd, (char *)messageFromClient, MAXLINE, MSG_WAITALL, (struct sockaddr*) &cliaddr, &length);
		
		if(bytesNumber > 0)
		{
			messageFromClient[bytesNumber] = '\0';
			printf("\n==============================================================\n");
			printf("\nMessage received from the client : %s\n", messageFromClient);
			
			newProcessID = fork();
			if(newProcessID == 0)
			{
				new_date_and_time();
				printf("Message sent to the client : %s\n", messageToClient);
				printf("\n==============================================================\n");
				sendto(sockfd, (const char *)messageToClient, strlen(messageToClient), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, length);
				close(sockfd);
        		exit(0);
			}
		}
	}
	return 0;
}

void new_date_and_time()
{
    char *day, *month, *year, *hour, *minute, *second;
    day     =   (char*)malloc(2 * sizeof(char));
    month   =   (char*)malloc(2 * sizeof(char));
    year    =   (char*)malloc(4 * sizeof(char));
    hour    =   (char*)malloc(2 * sizeof(char));
    minute  =   (char*)malloc(2 * sizeof(char));
    second  =   (char*)malloc(2 * sizeof(char));

    strncpy (day, messageFromClient + 0, 2);
    strncpy (month, messageFromClient + 3, 2);
    strncpy (year, messageFromClient + 6, 4);
    strncpy (hour, messageFromClient + 11, 2);
    strncpy (minute, messageFromClient + 14, 2);
    strncpy (second, messageFromClient + 17, 2);
    commandParameter =  *(messageFromClient + 21);

    day[2]      =   '\0';
    month[2]    =   '\0';
    year[4]     =   '\0';
    hour[2]     =   '\0';
    minute[2]   =   '\0';
    second[2]   =   '\0';

    end_date.day    = atoi(day);
    end_date.month  = atoi(month);
    end_date.year   = atoi(year);
    end_date.hour   = atoi(hour);
    end_date.minute = atoi(minute);
    end_date.second = atoi(second);
    
    tm_end_date.tm_mday = end_date.day; 
  	tm_end_date.tm_mon = end_date.month - 1; 
  	tm_end_date.tm_year = end_date.year - 1900;
    tm_end_date.tm_hour = end_date.hour + 2;  
    tm_end_date.tm_min = end_date.minute;  
    tm_end_date.tm_sec = end_date.second;
    
    time_t tm_end_time = mktime(&tm_end_date);
    printf("\nDate and time received: %s", asctime(gmtime(&tm_end_time)));
    
    tm_start_date.tm_mday = 1; 
  	tm_start_date.tm_mon = 0; 
  	tm_start_date.tm_year = 70;
    tm_start_date.tm_hour = 0 + 2;  
    tm_start_date.tm_min = 0;  
    tm_start_date.tm_sec = 0;
    
    time_t tm_start_time = mktime(&tm_start_date);
    printf("Reference date and time: %s\n", asctime(gmtime(&tm_start_time)));
    
    secondsDifference = difftime(tm_end_time, tm_start_time);
    
    char temporaryMessage[32];
    switch(commandParameter)
    {
    	case 's':
    		sprintf(temporaryMessage, "%lu", secondsDifference);
    		strcat(temporaryMessage, " seconds");
    	break;
    	case 'm':
    		sprintf(temporaryMessage, "%lu", secondsDifference/60);
    		strcat(temporaryMessage, " months");
    	break;
    	case 'z':
    		sprintf(temporaryMessage, "%lu", secondsDifference/86400);
    		strcat(temporaryMessage, " days");
    	break;
    	case 'a':
    		sprintf(temporaryMessage, "%lu", secondsDifference/31104000);
    		strcat(temporaryMessage, " years");
    	break;
    }
    messageToClient = (char*)malloc(strlen(temporaryMessage));
    strcpy(messageToClient, temporaryMessage);
    messageToClient[strlen(temporaryMessage)] = '\0';
    
}
