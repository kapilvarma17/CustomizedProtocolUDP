/*
Computer Networks Programming Assignment 2
Programming Assignment 2: Client using customized protocol on top of UDP protocol for requesting identification from server for access permission to the network

Note: I ran this program in the Design Center: Linux Lab

Name: KAPIL VARMA
ID: 1483983
*/

The program contains two files one for the server and one for the client. Other than this it contains screenshots for the output of the program and also the input file for the data to be given by the client and the database file which contains the data to verify the details.

To run the program follow these steps

1.Compile the server so that the client can send its data. Compile Assignment2Server.c file by using the comand “gcc -o Assignment2Server Assignment2Server.c”   (if this command throws error then use option -std=c99 or -std=gnu99 to compile your code)
 
2. The next step is to open another terminal and then compile the client by running the command: “gcc -o Assignment2Client Assignment2Client.c" (if this command throws error then use option -std=c99 or -std=gnu99 to compile your code)

3.Now we need to execute the server first by using the command "./Assingment2Server"

4.Similarly we need to also run the client and begin transmitting packets by using the command "./Assignment2Client"

The program has now run successfully.
