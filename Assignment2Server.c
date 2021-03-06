/*
Computer Networks
Programming Assignment 2: Client using customized protocol on top of UDP
protocol for requesting identification from server for access permission to the network
Name: KAPIL VARMA
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//Primitives
#define PORT 8080
#define LENGTH 10

//category codes
#define PAID 0XFFFB
#define NOTPAID 0XFFF9
#define NOTEXIST 0XFFFA

// Access Permission Request Packet Definition
struct RequestPacket{
	uint16_t startPacketID;
	uint8_t clientID;
	uint16_t AccPer;
	uint8_t segmentNumber;
	uint8_t length;
	uint8_t technology;
	unsigned int sourceSubscriberNum;
	uint16_t endPacketID;
};


// Response Packet Definition
struct ResponsePacket {
	uint16_t startPacketID;
	uint8_t clientID;
	uint16_t data;
	uint8_t segmentNumber;
	uint8_t length;
	uint8_t technology; //payload
	unsigned int sourceSubscriberNum; //payload
	uint16_t endPacketID;
};

// Verification database content
struct SubscriberDatabase {
	unsigned long subscriberNumber;
	uint8_t technology;
	int category; //paid, not paid, not found

};

// Creating a response packet for the client
struct ResponsePacket SetResPacket (struct RequestPacket reqP) {
	struct ResponsePacket rp;
	rp.startPacketID = reqP.startPacketID;
	rp.clientID = reqP.clientID;
	rp.segmentNumber = reqP.segmentNumber;
	rp.length = reqP.length;
	rp.technology = reqP.technology;
	rp.sourceSubscriberNum = reqP.sourceSubscriberNum;
	rp.endPacketID = reqP.endPacketID;
	return rp;
}


// Function to print the contents of the packet
void show (struct RequestPacket rqPt ) {
	printf("\nPACKET ID : %x\n",rqPt.startPacketID);
	printf("CLIENT ID : %hhx\n",rqPt.clientID);
	printf("ACCESS PERMISSION : %x\n",rqPt.AccPer);
	printf("SEGMENT NUMBER : %d \n",rqPt.segmentNumber);
	printf("LENGTH : %d\n",rqPt.length);
	printf("TECHNOLOGY : %d \n", rqPt.technology);
	printf("SOURCE SUBSCRIBER NUMBER : %u \n",rqPt.sourceSubscriberNum);
	printf("END OF REQUEST PACKET ID : %x \n",rqPt.endPacketID);
}

// Function to read the database file
void readFile (struct SubscriberDatabase subscriberDB[]) {


	char line[30];
	int i = 0;
	FILE *filePointer;
  //open the database file
	filePointer = fopen("Verification_Database.txt", "rt");

	if(filePointer == NULL)
	{
		printf("******* Verfication Database file could not be opened*******\n");
		return;
	}
	while (fgets(line, sizeof(line), filePointer) != NULL)
	{
		char * split=NULL;
		split = strtok(line," ");
		subscriberDB[i].subscriberNumber = (unsigned) atol(split);
		split = strtok(NULL," ");
		subscriberDB[i].technology = atoi(split);
		split = strtok(NULL," ");
		subscriberDB[i].category = atoi(split);
		i++;
	}
	fclose(filePointer);
}


//Function to verify if the subscriber existes in the database
int verify (struct SubscriberDatabase subscriberDB[],unsigned int subscriberNumber,uint8_t technology) {
	int value = -1;
	for (int j = 0; j < LENGTH;j++) {
		if (subscriberDB[j].subscriberNumber == subscriberNumber && subscriberDB[j].technology == technology) {
			return subscriberDB[j].category;
		}
    else if (subscriberDB[j].subscriberNumber == subscriberNumber && subscriberDB[j].technology != technology)
      return 2;
	}
	return value;
}

//Calling function main
int main(int argc, char**argv){

  struct RequestPacket reqPacket;
	struct ResponsePacket resPacket;

  struct SubscriberDatabase subscriberDB[LENGTH];
	readFile(subscriberDB);

  int sockfd,n; //scoket creation variables
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;
  //Create and Verify sockets
	sockfd=socket(AF_INET,SOCK_DGRAM,0);
  bzero(&serverAddr,sizeof(serverAddr));
  //Assign IP, PORT
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serverAddr.sin_port=htons(PORT);
  //Binding created socket to IP
  bind(sockfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr));
	addr_size = sizeof serverAddr;
	printf("*******THE SERVER IS INITIALIZED*******\n");

  for (;;) {
		//Receiving the Packet from client
		n = recvfrom(sockfd,&reqPacket,sizeof(struct RequestPacket),0,(struct sockaddr *)&serverStorage, &addr_size);
    printf("**************\n");
    show(reqPacket);
		if(reqPacket.segmentNumber == 11) {
			exit(0);
		}

		if(n > 0 && reqPacket.AccPer == 0XFFF8) {
			//Setting the repsonse packet
			resPacket = SetResPacket(reqPacket);

			int value = verify(subscriberDB,reqPacket.sourceSubscriberNum,reqPacket.technology);
			if(value == 0) {
				resPacket.data = NOTPAID;
				printf("*******SUBSCRIBER NOT PAID*******\n");
			}
      else if(value == -1) {
				printf("*******SUBSCRIBER DOES NOT EXIST*******\n");
				resPacket.data = NOTEXIST;
			}
			else if(value == 1) {
				printf("*******SUBSCRIBER PAID*******\n");
				resPacket.data = PAID;
			}
      else{
        printf("*******TECHNOLOGY OF SUBSCRIBER DOES NOT EXIST*******\n");
        resPacket.data = NOTEXIST;
      }
			// Sendinf response packet back to client
			sendto(sockfd,&resPacket,sizeof(struct ResponsePacket),0,(struct sockaddr *)&serverStorage,addr_size);
		}
		n = 0; //resetting data
		printf("\n ---------------------------  NEXT PACKET STARTS HERE ---------------------------  \n");
	}
}
