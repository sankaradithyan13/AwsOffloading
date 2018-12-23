#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <netinet/in.h>
using namespace std;
#define SERVERBPORT 22227 //server A static port number declaration
#define AWSPORT 24227 //AWS server static port number declaration
int main( void )
{
    //initializing all variables
    int fd; //variable to store socket descriptor
    ifstream file("database_b.csv"); //take input database file
	ifstream file1("database_b.csv"); //take a copy of the database file
	string value,data; //initialize variables
	string link[200], bw[200], len[200], velocity[200], npower[200], val[600]; //initialize more variables
	int i=0, c1=0, c2=0, c3=0, c4=0,a=0; //initialize int variables

    //server A UDP socket
    if ( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) //create a socket and store socket descriptor in variable fd
    { 
        perror( "Server B socket creation failed" ); //return error if there is error in socket creation
        return 1;
    }

    //server A details
    struct sockaddr_in serverBaddr; //initialize structure for server A
    memset( &serverBaddr, 0, sizeof(serverBaddr) ); //clear the memory contents of structure
    serverBaddr.sin_family = AF_INET; //address family is internet
    serverBaddr.sin_port = htons(SERVERBPORT); //port number of server A is static and defined above
    serverBaddr.sin_addr.s_addr = htonl( 0x7f000001 ); //IP address of server A
    printf("The server B is up and running using UDP on port <%d>\n",ntohs(serverBaddr.sin_port)); //print server is running on successful creation

    //binding server A address
    if ( bind(fd, (struct sockaddr *)&serverBaddr, sizeof(serverBaddr)) < 0 ) //bind the address of the server A to the socket descriptor
    { 
        perror( "Server B bind failed" ); //return error if there is error in binding socket
        return 1;
    }

    //aws details
    struct sockaddr_in awsaddr; //initialize structure for AWS server
    memset( &awsaddr, 0, sizeof(awsaddr) ); //clear the memory contents of structure
    int slenAWS = sizeof(awsaddr);
	awsaddr.sin_family = AF_INET; //address family is internet
	awsaddr.sin_port = htons(AWSPORT); //port number of AWS is static and defined above
	awsaddr.sin_addr.s_addr = htonl( 0x7f000001 ); //IP address of server A

    //fetch link ID
    while(file1.good()) //loop continuously until we get input from the database file
    {
    	if(getline(file1,value)); //get the input line by line and store in variable value to get link ID
    	{
    		istringstream ss(value); //store a sequence of characters from file1
			getline(ss,value,','); //split the values based on the comma ',' delimiter
			link[i] = value; //these are the values of link IDs
			i=i+1;	//increment value of count to store each link value in a location of a string array
    	}
    }

    //fetch other parameters
    while(file.good()) //loop continuously until we get input from the database file
    {
    	getline(file,data,','); //split the values based on the comma ',' delimiter
		istringstream ss(data); //store a sequence of characters from file
		getline(ss,data,'\n'); //split value based on newline '\n' delimiter
		val[a] = data; //store the data in an array
		a=a+1; //increment value of array location
    }
    for(int k=1;k<a;k++)
    {
    	if(k%4 == 1) //to extract and store the value of bandwidth from database
    	{
    		bw[c1] = val[k];
    		c1 = c1 + 1;
    	}
    	else if(k%4 == 2) //to extract and store the value of link length from database
    	{
    		len[c2] = val[k];
    		c2 = c2 + 1;
    	}
    	else if(k%4 == 3) //to extract and store the value of velocity of propagation from database
    	{
    		velocity[c3] = val[k];
    		c3 = c3 + 1;
    	}
    	else if(k%4 == 0) //to extract and store the value of noise power from database
    	{
    		npower[c4] = val[k];
    		c4 = c4+1;
    	}
    }
    while(1) //while loop to keep server continuously running to listen for inputs from client
    {
		char buffer[200]=""; //buffer for storing values received
    	int count = 0; //initialize count at every iteration
		//int length = recvfrom(fd,buffer,sizeof(buffer),0, (struct sockaddr *)&awsaddr, (socklen_t *)&slenAWS); //receive data from aws server
		if ( recvfrom(fd,buffer,sizeof(buffer),0, (struct sockaddr *)&awsaddr, (socklen_t *)&slenAWS) < 0 ) 
		{
			perror( "Receive from AWS server failed" ); //display error if the receive fails 
	    }
        //buffer[length] = '\0'; //set the last value of the received data to '\0' to indicate end of data
        printf( "The server B received input <%s>\n", buffer ); //display the message on the console of Server A
	    for(int j=0;j<i;j++)
	    {
	    	if(link[j] == buffer) //if link id received from client matches the one extracted from the database
	    	{	
				count = count+1; //increment count which acts as a flag
	    		if(sendto(fd,"ServerB",10, 0, (struct sockaddr *)&awsaddr, sizeof(awsaddr)) > 0) //send valid keyword to aws server if data is found in database
	    		{
	    			cout<<"The server B has found <1> match"<<endl; //display when match is found
		    		const void * bw_1 = bw[j].c_str(); //converting string to const void* for sending data
		    		const void * len_1 = len[j].c_str(); //converting string to const void* for sending data
		    		const void * velocity_1 = velocity[j].c_str(); //converting string to const void* for sending data
		    		const void * npower_1 = npower[j].c_str(); //converting string to const void* for sending data
		    		if (sendto( fd, bw_1, 100, 0, (struct sockaddr *)&awsaddr, sizeof(awsaddr)) < 0 ) //send bandwidth of link to aws server
		    		{
						perror( "Sending failed to AWS Server" ); //print error if sending failed
					}
					if (sendto( fd, len_1, 100, 0, (struct sockaddr *)&awsaddr, sizeof(awsaddr)) < 0 ) //send length of link to aws server
		    		{
						perror( "Sending failed to AWS Server" ); //print error if sending failed
					}
					if (sendto( fd, velocity_1, 100, 0, (struct sockaddr *)&awsaddr, sizeof(awsaddr)) < 0 ) //send velocity of propagation to aws server
		    		{
						perror( "Sending failed to AWS Server" ); //print error if sending failed
					}
					if (sendto( fd, npower_1, 100, 0, (struct sockaddr *)&awsaddr, sizeof(awsaddr)) < 0 ) //send Noise power of link to aws server
		    		{
						perror( "Sending failed to AWS Server" ); //print error if sending failed
					}
					printf( "The Server B finished sending the output to AWS\n"); //display message that server A has finished sending output
				}
                else
                {
                    perror("Sending message found to AWS server failed"); //if sending fails
                }                
	    	}
	    }
	    //cout<<count<<endl;
		if(count == 0) //if flag value is 0, then there was no match found
		{
            if(sendto(fd,"notServerB",10, 0, (struct sockaddr *)&awsaddr, sizeof(awsaddr))>0) //send keyword to aws server if data is not found in database
            {
                cout<<"The server B has found <0> match"<<endl; //display when no match is found
                printf( "The Server B finished sending the output to AWS\n"); //display message that server A has finished sending output
            }
            else
            {
                perror("Sending message not found to AWS server failed"); //if sending fails
            }
	   }
    }
	close( fd ); //close socket
}