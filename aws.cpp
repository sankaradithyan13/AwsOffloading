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
#define LOCALPORT 24227
#define SERVERAPORT 21227
#define SERVERBPORT 22227
#define SERVERCPORT 23227
#define AWSTCPPORT 25227
#define AWSTCPMONPORT 26227
int main()
{
    int fd,counta=0,countb=0;
    if ( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
    {
        perror("udp socket failed");
        return 1;
    }
    int tcp_socket, tcp_mon;
	
	if ( (tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) 
	{
        perror("tcp client socket failed");
        return 1;
    }

    if ( (tcp_mon = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) 
    {
        perror("tcp monitor socket failed");
        return 1;
    }
	//define the aws tcp client address
	struct sockaddr_in awstcpaddr;
	awstcpaddr.sin_family = AF_INET;
	awstcpaddr.sin_port = htons(AWSTCPPORT);
	awstcpaddr.sin_addr.s_addr = htonl( 0x7f000001 );

	//bind the socket to our specified IP and port
	bind(tcp_socket, (struct sockaddr *) &awstcpaddr, sizeof(awstcpaddr));

	//define the aws tcp monitor address
	struct sockaddr_in awstcpmonaddr;
	awstcpmonaddr.sin_family = AF_INET;
	awstcpmonaddr.sin_port = htons(AWSTCPMONPORT);
	awstcpmonaddr.sin_addr.s_addr = htonl( 0x7f000001 );

	bind(tcp_mon, (struct sockaddr *) &awstcpmonaddr, sizeof(awstcpmonaddr));


 	//aws address
    struct sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl( 0x7f000001 );
	localaddr.sin_port = htons(LOCALPORT);
	bind(fd, (struct sockaddr *)&localaddr, sizeof(localaddr));
    printf("The AWS is up and running\n");

	
    //details for server A
    struct sockaddr_in serverAaddr;
    memset( &serverAaddr, 0, sizeof(serverAaddr) );
    int slenA = sizeof(serverAaddr);
    serverAaddr.sin_family = AF_INET;
    serverAaddr.sin_port = htons(SERVERAPORT);              
    serverAaddr.sin_addr.s_addr = htonl( 0x7f000001 ); 

	//details for server B
    struct sockaddr_in serverBaddr;
    memset( &serverBaddr, 0, sizeof(serverBaddr) );
    int slenB = sizeof(serverBaddr);
    serverBaddr.sin_family = AF_INET;
    serverBaddr.sin_port = htons(SERVERBPORT);              
    serverBaddr.sin_addr.s_addr = htonl( 0x7f000001 );  

    //details for server C
	struct sockaddr_in serverCaddr;
    memset( &serverCaddr, 0, sizeof(serverCaddr) );
    int slenC = sizeof(serverCaddr);
    serverCaddr.sin_family = AF_INET;
    serverCaddr.sin_port = htons(SERVERCPORT);              
    serverCaddr.sin_addr.s_addr = htonl( 0x7f000001 );  

    listen(tcp_mon, 5);
	int monitor_socket;
	monitor_socket = accept(tcp_mon, NULL, NULL);
    while(1)
    {
    	char bandwidth[200]="";
		char length[200]="";
		char velocity[200]="";
		char npower[200]="";

		char bandwidth_b[200]="";
		char length_b[200]="";
		char velocity_b[200]="";
		char npower_b[200]="";

		char tprop[150] = "";
		char ttrans[150] = "";
		char channelcap[150] = "";
		char delay[150] = "";

    	listen(tcp_socket, 5);
		int client_socket;
		client_socket = accept(tcp_socket, NULL, NULL);
		char c_link[100], c_size[100], c_spower[100];

    	recv(client_socket, &c_link, 10, 0);
		recv(client_socket, &c_size, 10, 0);
		recv(client_socket, &c_spower, 10, 0);
	    printf("The AWS received link ID = <%s>, size = <%s> and power = <%s> from the client using TCP over port <%d>\n",c_link,c_size,c_spower,ntohs(awstcpaddr.sin_port));
    	//char mon[200],timep[100]="";
		//recv(monitor_socket, &timep, 5, 0);
		if (send(monitor_socket, c_link, 10, 0) < 0 ) 
		{
			perror( "sendto link ID failed to monitor" );
		}
		if (send(monitor_socket, c_size, 10, 0) < 0 )
		{
			perror( "sendto file size failed to monitor" );
		}
		if (send(monitor_socket, c_spower, 10, 0) < 0 )
		{
			perror( "sendto power info failed to monitor" );
		}
		printf("The AWS sent link ID = <%s>, size = <%s> and power = <%s> to the monitor using TCP over port <%d>\n",c_link,c_size,c_spower,ntohs(awstcpmonaddr.sin_port));
		//cout<<"dadadads"<<endl;
		if (sendto( fd, c_link, 10, 0, (struct sockaddr *)&serverAaddr, sizeof(serverAaddr)) < 0 ) 
		{
			perror( "sendto failed to server A" );
		}
		else
		{
			printf( "The AWS sent link ID = <%s> to Backend-server A using UDP over port <%d>\n",c_link,ntohs(localaddr.sin_port));
		}
		counta = 0;
		countb = 0;
		char min[100];
		int data_a=0,data_b=0,bal_a=0,bal_anot=0,bal_b=0;
		if(data_a = recvfrom(fd,min,sizeof(min),0, (struct sockaddr *)&serverAaddr, (socklen_t *)&slenA))
		{	
			if (sendto( fd, c_link, 10, 0, (struct sockaddr *)&serverBaddr, sizeof(serverBaddr)) < 0 ) 
			{
				perror( "sendto failed to server B" );
			}
			else
				printf( "The AWS sent link ID = <%s> to Backend-server B using UDP over port <%d>\n",c_link,ntohs(localaddr.sin_port));
			//cout<<min<<endl;
			bal_a = strcmp(min,"ServerA");
			if (bal_a == 0)
			{
				recvfrom(fd,bandwidth,sizeof(bandwidth),0, (struct sockaddr *)&serverAaddr, (socklen_t *)&slenA);
				recvfrom(fd,length,sizeof(length),0, (struct sockaddr *)&serverAaddr, (socklen_t *)&slenA);
				recvfrom(fd,velocity,sizeof(velocity),0, (struct sockaddr *)&serverAaddr, (socklen_t *)&slenA);
				recvfrom(fd,npower,sizeof(npower),0, (struct sockaddr *)&serverAaddr, (socklen_t *)&slenA);
				printf("The AWS received <1> matches from backend server A using UDP over port <%d>\n",ntohs(localaddr.sin_port));
				printf("The AWS received <0> matches from backend server B using UDP over port <%d>\n",ntohs(localaddr.sin_port));
			    //printf( "The AWS server received input <%s> <%s> <%s> <%s> from server A\n", bandwidth,length,velocity,npower );
			    sendto( fd, bandwidth, sizeof(bandwidth), 0, (struct sockaddr *)&serverCaddr, sizeof(serverCaddr));
			    sendto( fd, length, sizeof(length), 0, (struct sockaddr *)&serverCaddr, sizeof(serverCaddr));
			    sendto( fd, velocity, sizeof(velocity), 0, (struct sockaddr *)&serverCaddr, sizeof(serverCaddr));
			    sendto( fd, npower, sizeof(npower), 0, (struct sockaddr *)&serverCaddr, sizeof(serverCaddr));
			    counta = counta + 1;
			    data_b = recvfrom(fd,min,sizeof(min),0, (struct sockaddr *)&serverBaddr, (socklen_t *)&slenB);
			}
			if (data_a == -1)
				cout<<"Receive from server A error"<<endl;
			bal_anot = strcmp(min,"notServerA");
			if(bal_anot == 0)
			{
				data_b = recvfrom(fd,min,sizeof(min),0, (struct sockaddr *)&serverBaddr, (socklen_t *)&slenB);
				bal_b = strcmp(min,"ServerB");
				if (bal_b == 0)
				{
					recvfrom(fd,bandwidth_b,sizeof(bandwidth_b),0, (struct sockaddr *)&serverBaddr, (socklen_t *)&slenB);
					recvfrom(fd,length_b,sizeof(length_b),0, (struct sockaddr *)&serverBaddr, (socklen_t *)&slenB);
					recvfrom(fd,velocity_b,sizeof(velocity_b),0, (struct sockaddr *)&serverBaddr, (socklen_t *)&slenB);
					recvfrom(fd,npower_b,sizeof(npower_b),0, (struct sockaddr *)&serverBaddr, (socklen_t *)&slenB);
					printf("The AWS received <0> matches from backend server A using UDP over port <%d>\n",ntohs(localaddr.sin_port));
					printf("The AWS received <1> matches from backend server B using UDP over port <%d>\n",ntohs(localaddr.sin_port));
					//printf( "The AWS server received input <%s> <%s> <%s> <%s> from server A\n", bandwidth,length,velocity,npower );
				    sendto( fd, bandwidth_b, sizeof(bandwidth_b), 0, (struct sockaddr *)&serverCaddr, sizeof(serverCaddr));
				    sendto( fd, length_b, sizeof(length_b), 0, (struct sockaddr *)&serverCaddr, sizeof(serverCaddr));
				    sendto( fd, velocity_b, sizeof(velocity_b), 0, (struct sockaddr *)&serverCaddr, sizeof(serverCaddr));
				    sendto( fd, npower_b, sizeof(npower_b), 0, (struct sockaddr *)&serverCaddr, sizeof(serverCaddr));
				    countb = countb + 1;
				}
				if(data_b == -1)
					cout<<"Receive from server B error"<<endl;
			}
			//cout<<counta<<endl<<countb<<endl;
			if (counta == 0 && countb==0)
			{
				printf("The AWS received <0> matches from Backend-server <A> using UDP over port <%d>\n",ntohs(localaddr.sin_port));
				printf("The AWS received <0> matches from Backend-server <B> using UDP over port <%d>\n",ntohs(localaddr.sin_port));
				printf("The AWS sent No Match to the monitor and the client using TCP over ports <%d> and <%d>, respectively\n",ntohs(awstcpmonaddr.sin_port),ntohs(awstcpaddr.sin_port));
				if (send(client_socket,"n", 10, 0) < 0 )
				{
					perror( "sendto no match info failed to client" );
				}

				if (send(monitor_socket, "n", 10, 0) < 0 )
				{
					perror( "sendto power info failed to monitor" );
				}
			}			
			else
			{
				if (sendto( fd, c_link, 10, 0, (struct sockaddr *)&serverCaddr, sizeof(serverCaddr)) < 0 ) 
				{
					perror( "sendto link ID failed to server C" );
				}
				if (sendto( fd, c_size, 10, 0, (struct sockaddr *)&serverCaddr, sizeof(serverCaddr)) < 0 )
				{
					perror( "sendto file size failed to server C" );
				}
				if (sendto( fd, c_spower, 10, 0, (struct sockaddr *)&serverCaddr, sizeof(serverCaddr)) < 0 )
				{
					perror( "sendto power info failed to server C" );
				}
				printf("The AWS sent link ID = <%s>, size = <%s> power = <%s> and link information to Backend-server C using UDP over port <%d>\n",c_link,c_size,c_spower,ntohs(localaddr.sin_port));
				
				recvfrom(fd,tprop,sizeof(tprop),0, (struct sockaddr *)&serverCaddr, (socklen_t *)&slenC);
				recvfrom(fd,ttrans,sizeof(ttrans),0, (struct sockaddr *)&serverCaddr, (socklen_t *)&slenC);
				recvfrom(fd,channelcap,sizeof(channelcap),0, (struct sockaddr *)&serverCaddr, (socklen_t *)&slenC);
				recvfrom(fd,delay,sizeof(delay),0, (struct sockaddr *)&serverCaddr, (socklen_t *)&slenC);
				printf("The AWS received outputs from Backend-server C using UDP over port <%d>\n", ntohs(localaddr.sin_port));
				//printf("%s %s %s %s\n",tprop,ttrans,channelcap,delay );
				if (send(client_socket, delay, 10, 0) < 0 )
				{
					perror( "sendto power info failed to client" );
				}
				stringstream delay_i(delay);
		        float delay_f=0;
		        delay_i>>delay_f;
				printf("The AWS sent delay = <%.2f>ms to the client using TCP over port <%d>\n",delay_f,ntohs(awstcpaddr.sin_port));
				if (send(monitor_socket, tprop, 10, 0) < 0 )
				{
					perror( "sendto power info failed to monitor" );
				}
				if (send(monitor_socket, ttrans, 10, 0) < 0 )
				{
					perror( "sendto power info failed to monitor" );
				}
				if (send(monitor_socket, delay, 10, 0) < 0 )
				{
					perror( "sendto power info failed to monitor" );
				}
				printf("The AWS sent detailed results to the monitor using TCP over port <%d>\n",ntohs(awstcpmonaddr.sin_port));
			}
		}
	}
    close(fd);
	close(tcp_socket);
}