#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <netinet/in.h>
#include <math.h>
using namespace std;
#define SERVERCPORT 23227
#define AWSPORT 24227
int main( void )
{
    int fd;
    if ( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
    {
        perror( "socket failed" );
        return 1;
    }

    struct sockaddr_in serveraddr;
    //memset( &serveraddr, 0, sizeof(serveraddr) );
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVERCPORT);
    serveraddr.sin_addr.s_addr = htonl( 0x7f000001 );

    //aws details
    struct sockaddr_in awsaddr;
    //memset(&awsaddr, 0, sizeof(awsaddr));
    int slenAWS = sizeof(awsaddr);
    awsaddr.sin_family = AF_INET;
    awsaddr.sin_port = htons(AWSPORT);
    awsaddr.sin_addr.s_addr = htonl( 0x7f000001 );

    printf("The server C is up and running using UDP on port <%d>\n",ntohs(serveraddr.sin_port));
    if ( bind(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0 ) 
    {
        perror( "bind failed" );
        return 1;
    }

    while(1)
    {
        char buffer1[100]="";
        char buffer2[100]="";
        char buffer3[100]="";
        char bandwidth[200]="";
        char length_link[200]="";
        char velocity[200]="";
        char npower[200]="";
        int length = recvfrom(fd,bandwidth,sizeof(bandwidth),0, (struct sockaddr *)&awsaddr, (socklen_t *)&slenAWS);
        if ( length < 0 ) {
            perror( "recvfrom failed" );
            //break;
        }
        bandwidth[length] = '\0';
        length = recvfrom(fd,length_link,sizeof(length_link),0, (struct sockaddr *)&awsaddr, (socklen_t *)&slenAWS);
        if ( length < 0 ) {
            perror( "recvfrom failed" );
            //break;
        }
        length_link[length] = '\0';
        length = recvfrom(fd,velocity,sizeof(velocity),0, (struct sockaddr *)&awsaddr, (socklen_t *)&slenAWS);
        if ( length < 0 ) {
            perror( "recvfrom failed" );
            //break;
        }
        velocity[length] = '\0';
        length = recvfrom(fd,npower,sizeof(npower),0, (struct sockaddr *)&awsaddr, (socklen_t *)&slenAWS);
        if ( length < 0 ) {
            perror( "recvfrom failed" );
            //break;
        }
        npower[length] = '\0';

        length = recvfrom(fd,buffer1,sizeof(buffer1),0, (struct sockaddr *)&awsaddr, (socklen_t *)&slenAWS);
        if ( length < 0 ) {
            perror( "recvfrom failed" );
            //break;
        }
        buffer1[length] = '\0';
        length = recvfrom(fd,buffer2,sizeof(buffer2),0, (struct sockaddr *)&awsaddr, (socklen_t *)&slenAWS);
        if ( length < 0 ) {
            perror( "recvfrom failed" );
            //break;
        }
        buffer2[length] = '\0';
        length = recvfrom(fd,buffer3,sizeof(buffer3),0, (struct sockaddr *)&awsaddr, (socklen_t *)&slenAWS);
        if ( length < 0 ) {
            perror( "recvfrom failed" );
            //break;
        }
        buffer3[length] = '\0';
        printf( "The server C received link information of link <%s>, file size <%s> and signal power <%s>\n", buffer1, buffer2, buffer3);
        stringstream link_i(buffer1);
        stringstream size_i(buffer2);
        stringstream spower_i(buffer3);
        float size=0, spower=0;
        int link = 0;
        link_i>>link;
        size_i>>size;
        spower_i>>spower;

        stringstream bw_i(bandwidth);
        stringstream len_i(length_link);
        stringstream vel_i(velocity);
        stringstream np_i(npower);
        float bw=0,len=0,vel=0,np=0;
        bw_i>>bw;
        len_i>>len;
        vel_i>>vel;
        np_i>>np;

        float Tprop, Ttrans, De2e, chcap, snr,spower_reg,npower_reg;

        Tprop = len*1000/(vel*10000000)*1000;
        spower_reg = pow(10,(spower/10))/1000;
        npower_reg = pow(10,(np/10))/1000;
        snr = spower_reg/npower_reg;
        //tot = 1 + snr;
        chcap = bw * 1000000 * log2 (1+snr);
        Ttrans = size/chcap*1000;

        De2e = Tprop + Ttrans;
        
        cout<<link<<endl<<size<<endl<<spower<<endl<<bw<<endl<<len<<endl<<vel<<endl<<np<<endl<<Tprop<<endl<<Ttrans<<endl<<chcap<<endl<<De2e<<endl;
        if(Ttrans<0.01)
            Ttrans=0.00;
        if(Tprop<0.01)
            Tprop=0.00;
        if(De2e<0.01)
            De2e=0.00;

        printf("The server C finished calculation for link <%d>\n",link);
        ostringstream strs;
        strs << Tprop;
        string tp = strs.str();
        ostringstream strs1;
        strs1 << Ttrans;
        string tt = strs1.str();
        ostringstream strs2;
        strs2 << chcap;
        string c = strs2.str();
        ostringstream strs3;
        strs3 << De2e;
        string d = strs3.str();

        const void * Tprop_c = tp.c_str();
        const void * Ttrans_c = tt.c_str();
        const void * chcap_c = c.c_str();
        const void * De2e_c = d.c_str();

        sendto( fd, Tprop_c, sizeof(Tprop_c), 0, (struct sockaddr *)&awsaddr, sizeof(awsaddr));
        sendto( fd, Ttrans_c, sizeof(Ttrans_c), 0, (struct sockaddr *)&awsaddr, sizeof(awsaddr));
        sendto( fd, chcap_c, sizeof(chcap_c), 0, (struct sockaddr *)&awsaddr, sizeof(awsaddr));
        sendto( fd, De2e_c, sizeof(De2e_c), 0, (struct sockaddr *)&awsaddr, sizeof(awsaddr));

        printf("The server C finished sending the output to AWS\n");
    }
    close( fd );
}