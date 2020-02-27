/*
Computer Networks
Programming Assignment 2: Client using customized protocol on top of UDP
protocol for requesting identification from server for access permission to the network
Name: KAPIL VARMA
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>
#include <stdint.h>
#include <string.h>

//Primitives
#define PORT 8080

//category Codes
#define PAID 0XFFFB
#define NOTPAID 0XFFF9
#define NOTEXIST 0XFFFA


// Structure of  a Request Packet
struct RequestPacket {
	uint16_t startPacketId;
	uint8_t clientId;
	uint16_t AccPer;
	uint8_t segmentNumber;
	uint8_t length;
	uint8_t technology;
	unsigned int sourceSubscriberNum;
	uint16_t endPacket;
};


// Structure of a response packet
struct ResponsePacket {
	uint16_t startPacketId;
	uint8_t clientId;
	uint16_t data;
	uint8_t segmentNumber;
	uint8_t length;
	uint8_t technology;
	unsigned int sourceSubscriberNum;
	uint16_t endPacket;
};


// Function to print the contents of the packet
void show (struct RequestPacket reqP) {
	printf("\nPACKET ID : %x\n", reqP.startPacketId);
	printf("CLIENT ID : %hhx\n", reqP.clientId);
	printf("ACCESS PERMISSION : %x\n", reqP.AccPer);
	printf("SEGMENT NUMBER : %d \n", reqP.segmentNumber);
	printf("LENGTH : %d\n", reqP.length);
	printf("TECHNOLOGY : %d \n", reqP.technology);
	printf("SUBSCRIBER NUMBER : %u \n", reqP.sourceSubscriberNum);
	printf("END DATA PACKET ID : %x \n", reqP.endPacket);
}


// To load Data in the request packet as mentioned
struct RequestPacket Set () {
	struct RequestPacket reqP;
	reqP.startPacketId = 0XFFFF;
	reqP.clientId = 0XFF;
	reqP.AccPer = 0XFFF8;
	reqP.endPacket = 0XFFFF;
	return reqP;
}

//Calling Function Main
int main(int argc, char**argv){

  struct RequestPacket reqPacket;
	struct ResponsePacket resPacket;

	char line[30];
	int i = 1;
	FILE *filePointer;
	int sockfd,n = 0;
	struct sockaddr_in clientAddr;
	socklen_t addr_size;

  //Create and Verify Sockets
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	struct timeval timeValue;
	// Timeout Value as required
	timeValue.tv_sec = 3;
	timeValue.tv_usec = 0;

	// verifying for connection to the server
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeValue,sizeof(struct timeval));
	int count = 0;
	if(sockfd < 0) {
		printf("*******SOCKET NOT RESPONDING*******\n");
	}
	bzero(&clientAddr,sizeof(clientAddr));
  //Assign IP and Port
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	clientAddr.sin_port=htons(PORT);
	addr_size = sizeof clientAddr ;

	// Setting data of the packet
	reqPacket = Set();

	//Reading input file
	filePointer = fopen("input.txt", "rt");
	if (filePointer == NULL)
	{
		printf("*******NOT ABLE TO OPEN FILE*******\n");
	}

	while (fgets(line, sizeof(line), filePointer) != NULL) {
		count = 0;
		n = 0;
		printf(" \n*******NEW REQUEST*******\n");
		char * split;

		split = strtok(line," ");
		reqPacket.length = strlen(split);
		reqPacket.sourceSubscriberNum = (unsigned) atoi(split);
		split = strtok(NULL," ");
		reqPacket.length += strlen(split);
		reqPacket.technology = atoi(split);
		split = strtok(NULL," ");
		reqPacket.segmentNumber = i;

		show(reqPacket);
		while (n <= 0 && count < 3) {
			// Sending packet, retry if fails
			sendto(sockfd,&reqPacket,sizeof(struct RequestPacket),0,(struct sockaddr *)&clientAddr,addr_size);
			// getting the response form the server
			n = recvfrom(sockfd,&resPacket,sizeof(struct ResponsePacket),0,NULL,NULL);
			if (n <= 0 ) {
				// If response not recieved the required time
				printf("*******TIMED OUT*******\n");
				count++;
			}
      //iF response is recieved
			else if (n > 0) {
				printf("Category : ");
				if(resPacket.data == NOTPAID) {
					printf("*******SUBSCRIBER NOT PAID*******\n"); //Payment not done
				}
				else if(resPacket.data == NOTEXIST ) { //Subsriber itself does nto exists
					printf("*******SUBSCRIBER DOES NOT EXIST*******\n");
				}
				else if(resPacket.data == PAID) {
					printf("*******ACCESS GRANTED TO NETWORK*******\n"); //IF PAID

				}
			}
		}
		// Print after three failed attempts
		if(count >= 3 ) {
			printf("*******THE SERVER DID NOT RESPOND*******");
			exit(0);
		}
		i++; //INCREMENT
		printf("\n ---------------------------  NEXT PACKET STARTS HERE ---------------------------  \n");
	}
	fclose(filePointer);
}
