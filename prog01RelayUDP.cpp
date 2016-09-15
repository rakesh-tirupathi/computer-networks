// ***** udprelay.cpp *****

//*******************************************************************
//
//
// Description: This is a UDP socket program that illustrates how a client 
//              (udpclient.cpp) talks to a UDP server via a relay server and sends a message
//              to the server and receives a response via a relay server.
//
// Editor/Platform:  vi/Linux
//
//
//
//  Input:      none/or user input
//
//  Output:     The client will display the message replied by this server
//              The server will display the message sent by a client
//
//  
//
//
//*******************************************************************
// ***** udprelaysocket1 *****
    

                             // header files

//#include <stdio.h>    //for printf(), ...
#include <stdlib.h>
#include <string.h>
//#include <sys/types.h>        //for data types
#include <sys/socket.h> //for socket(), connect(), ...
#include <unistd.h>     //for close()
#include <netinet/in.h> //for internet address family
#include <arpa/inet.h>  //for sockaddr_in, inet_addr()
#include <errno.h>      //for system error numbers
#include <iostream>     //for cin, cout...


#define MY_PORT_ID   54713     

#define MESSAGESIZE  500

using std::cout;
using std::cerr;

main( int argc, char *argv[])
{
   int sock1fd;  //socket declaration
   struct sockaddr_in my_addr, client_addr;  //addresses
   int nread,        //number of bytes read
       retcode,      //returned code from a function
       addrlen;      //address length
   
   char msg[MESSAGESIZE]; 

   int my_port_id = MY_PORT_ID;  //default port number   
   if (argc == 2)   // if command line with port number, take it
   {  
       my_port_id = atoi(argv[1]);
   }
   
   // ---------------------------------------------------------
   // Initialization:
   // ---------------------------------------------------------
   //cout << "Server: creating socket\n";
   if ( (sock1fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
   { 
       cerr << "Server: socket error: " << errno << "\n"; 
       exit(1); 
   }

   // cout << "Server: binding my local socket\n";
   memset( &my_addr, 0, sizeof(my_addr));    // Zero out structure
   my_addr.sin_family = AF_INET;        // Internet address family
   my_addr.sin_port = htons(my_port_id);    //My port
   my_addr.sin_addr.s_addr = htonl(INADDR_ANY);    // Any incoming interface

   // ---------------------------------------------------------
   // binding:
   // ---------------------------------------------------------
   if ( (bind(sock1fd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) )
   { 
       cerr << "Server: bind fail: " << errno << "\n"; 
       exit(2); 
   }   


   while (1)
   {
       // ---------------------------------------------------------
       // Wait for client's connection
       // --------------------------------------------------------- 
       cout << "\n\nWaiting for client's message....\n\n";
       addrlen = sizeof(client_addr);   // need to give it the buffer size for sender's address
       nread = recvfrom(sock1fd,msg, MESSAGESIZE, 0,
                  (struct sockaddr *) &client_addr, (socklen_t *) &addrlen);   

       if (nread >0) 
       {
           cout << "Receive from client (IP:" << inet_ntoa(client_addr.sin_addr) 
                << " port:" << ntohs(client_addr.sin_port)<<")\n>>"; 
           cout.write(msg, nread);
           cout << "\n";
           
            // ***** udprelaysocket2 *****
       //creation of relay socket2 for sending message to the final/destination server
      //Here the relay socket2 acts as a client
           
                    #define SERVER_PORT_ID  54713
                     
                     #define SERV_HOST_ADDR "147.97.156.238" 
                     
                     using std::cout;
                     using std::cerr;
                     using std::cin;
                     
                        int sock2fd, retcode, nread, addrlen;
                        int server_port_id = SERVER_PORT_ID;
                        char serv_host_addr[20]=SERV_HOST_ADDR;
                        struct sockaddr_in my_addr, server_addr;
                     
                        // ---------------------------------------------------------
                        // Initialization:
                        // ---------------------------------------------------------
                        //cout << "Client: creating socket\n";
                        if ( (sock2fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
                        { 
                            cerr << "Client: socket failed: "<< errno <<"\n"; 
                            exit(1); 
                        }
                     
                     
                        // cout << "client: binding my local socket\n";  // this is optional
                        memset( &my_addr, 0, sizeof(my_addr));       // Zero out structure
                        my_addr.sin_family = AF_INET;                // Internet address family
                        my_addr.sin_port = htons(0);                 // My port, 0 means any random available one
                        my_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
                     
                        // ---------------------------------------------------------
                        // binding:
                        // ---------------------------------------------------------
                        if ( (bind(sock2fd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) )
                        {
                            cerr << "client: bind fail: " << errno << "\n";
                            exit(2);
                        }
                     
                     
                        // ---------------------------------------------------------
                        // prepare server address
                        // ---------------------------------------------------------
                        // cout << "Client: creating addr structure for server\n";
                        memset( &server_addr, 0, sizeof(server_addr));
                        server_addr.sin_family = AF_INET;
                        server_addr.sin_port = htons(server_port_id);
                        //server_addr.sin_addr.s_addr = inet_addr(serv_host_addr);// old style
                        if (inet_aton(serv_host_addr, &(server_addr.sin_addr))==0) // get server addr   
                        {  // invalid server address
                            cerr << "Client: Invalid server address...\n";
                            exit(2);
                        }
                     
                     
                        // ---------------------------------------------------------
                        // Message Preparation  
                        // ---------------------------------------------------------
                        // cout << "Client: initializing message and sending\n";
                     
                        
                        cout<<"\n Please type a message to the server\n ";
                        //cin.getline(msg, MESSAGESIZE, '\n');
                     
                     
                        // ---------------------------------------------------------
                        // Transmission  
                        // ---------------------------------------------------------
                        retcode = sendto(sock2fd, msg, strlen(msg)+1 , 0,
                                       (struct sockaddr *) &server_addr, sizeof(server_addr));
                        if (retcode <= -1)
                        {
                            cerr << "client: sendto failed: " << errno << "\n";
                            exit(3); 
                        }   
                     
                        // ---------------------------------------------------------
                        // Get message from server 
                        // ---------------------------------------------------------
                        addrlen = sizeof(server_addr);   // specify the size of server_addr, this is important !!!!!
                        nread = recvfrom(sock2fd,msg, MESSAGESIZE, 0,
                                      (struct sockaddr *) &server_addr, (socklen_t *) &addrlen);
                        if (nread >0)
                        {
                            cout << "Client: server's response message is: ";
                            cout.write(msg, nread);
                            cout << "\n";
                        }
                     
                     
                        // ---------------------------------------------------------
                        // Termination
                        // ---------------------------------------------------------
                        /* close socket */
                        close(sock2fd);
                      //}


           // ---------------------------------------------------------
           // prepare a message and send to client
           // --------------------------------------------------------- 
           // send a message back to client
           // strcpy(msg, "Got it!!!");
           cout << "Please type a message back to client:\n";
           //nread = read(0, msg, MESSAGESIZE);    // read from keyboard
           //msg[nread]='\0';
           retcode = sendto(sock1fd,msg,strlen(msg)+1,0,
                     (struct sockaddr *) &client_addr, sizeof(client_addr));
           if (retcode <= -1)
           {
               cerr << "client: sendto failed: " << errno << "\n";
               exit(3);
           }
       }
                   
   }
   // ---------------------------------------------------------
   // Termination
   // ---------------------------------------------------------
   close(sock1fd);
}

