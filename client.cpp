#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
#define AWSPORT 25227
int main( int argc, char* argv[])
{
    int fd;
    if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) 
    {
        perror("Client socket failed");
        return 1;
    }

    //aws tcp address
    struct sockaddr_in awstcpaddr;
	awstcpaddr.sin_family = AF_INET;
	//int slenAWS = sizeof(awstcpaddr);
	awstcpaddr.sin_port = htons(AWSPORT);
	awstcpaddr.sin_addr.s_addr = INADDR_ANY;
    printf("The client is up and running\n");
    char delay[100]="";
	int connection_status = connect(fd, (struct  sockaddr*) &awstcpaddr, sizeof(awstcpaddr));
	if(connection_status == -1)
	{
		printf("There was an error in making a connection to the remote socket\n\n");
	}

	if (sendto( fd, argv[1], 10, 0, (struct sockaddr *)&awstcpaddr, sizeof(awstcpaddr)) < 0 ) {
		perror( "sendto link ID failed to AWS server" );
	}
	if (sendto( fd, argv[2], 10, 0, (struct sockaddr *)&awstcpaddr, sizeof(awstcpaddr)) < 0 )
	{
		perror( "sendto file size failed to AWS server" );
	}
	if (sendto( fd, argv[3], 10, 0, (struct sockaddr *)&awstcpaddr, sizeof(awstcpaddr)) < 0 )
	{
		perror( "sendto power info failed to AWS server" );
	}
	printf( "The client sent link ID = <%s>, size = <%s>, power = <%s> to AWS server using TCP over port <%d>\n",argv[1],argv[2],argv[3],ntohs(awstcpaddr.sin_port));
	recv(fd, &delay, 10, 0);
	
	int bal = strcmp(delay,"n");
	//cout<<bal;
	if(bal==0)
	{
		printf( "Found no matches for link <%s>\n",argv[1]);
	}
	else
	{
		stringstream delay_i(delay);
        float delay_f=0;
        delay_i>>delay_f;
		printf( "The delay for link <%s> is <%.2f>ms\n",argv[1],delay_f);
	}
	
    close( fd );
}