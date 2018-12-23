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
#define AWSTCPMONPORT 26227
int main( void)
{
    int fd;
    
    if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) 
    {
        perror("Monitor socket failed");
        //return 1;
    }

    //aws tcp address
    struct sockaddr_in awstcpaddr;
	awstcpaddr.sin_family = AF_INET;
	//int slenAWS = sizeof(awstcpaddr);
	awstcpaddr.sin_port = htons(AWSTCPMONPORT);
	awstcpaddr.sin_addr.s_addr = htonl( 0x7f000001 );
    printf("The monitor is up and running\n");

	int connection_status = connect(fd, (struct  sockaddr*) &awstcpaddr, sizeof(awstcpaddr));
	if(connection_status == -1)
	{
		printf("There was an error in making a connection to the remote socket\n\n");
	}
	while(1)
	{
		char tprop[100]="", ttrans[100]="", delay[100]="",c_link[100]="",c_size[100]="",c_spower[100]="";
		//int connection_status = connect(fd, (struct  sockaddr*) &awstcpaddr, sizeof(awstcpaddr));
		if (sendto( fd, "Hello", 10, 0, (struct sockaddr *)&awstcpaddr, sizeof(awstcpaddr)) < 0 ) 
		{
			perror( "sendto link ID failed to AWS server" );
		}
		recv(fd, &c_link, 10, 0);
		recv(fd, &c_size, 10, 0);
		recv(fd, &c_spower, 10, 0);
		printf( "The monitor received link ID = <%s>, size = <%s>, power = <%s> from the AWS\n",c_link,c_size,c_spower);
		recv(fd, &tprop, 10, 0);
		int bal = strcmp(tprop,"n");
		if(bal==0)
		{
			printf( "Found no matches for link <%s>\n",c_link);
		}
		else
		{
			recv(fd, &ttrans, 10, 0);
			recv(fd, &delay, 10, 0);
			stringstream delay_i(delay);
			stringstream tprop_i(tprop);
			stringstream ttrans_i(ttrans);
		    float delay_f=0,ttrans_f=0,tprop_f=0;
		    delay_i>>delay_f;
		    tprop_i>>tprop_f;
		    ttrans_i>>ttrans_f;
			printf( "The result for link <%s>\nTt = <%.2f>ms,\nTp = <%.2f>ms,\nDelay = <%.2f>ms\n",c_link,ttrans_f,tprop_f,delay_f);
		}
		
	}
    close(fd);

}