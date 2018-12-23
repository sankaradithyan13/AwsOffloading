CC=g++

CFLAGS=-g -Wall -Iinclude -Wno-parentheses 

all: 
	$(CC) -o serverAoutput serverA.cpp
	$(CC) -o serverBoutput serverB.cpp
	$(CC) -o serverCoutput serverC.cpp
	$(CC) -o awsoutput aws.cpp
	$(CC) -o monitoroutput monitor.cpp
	$(CC) -o client client.cpp

serverA.o: serverA.cpp
	./serverAoutput

serverB.o: serverB.cpp
	./serverBoutput 

serverC.o: serverC.cpp
	./serverCoutput

monitor.o: monitor.cpp
	./monitoroutput

aws.o: aws.cpp
	./awsoutput

clean:
	$(RM) serverAoutput serverBoutput serverCoutput awsoutput monitoroutput client
