// ***** tcprelay.cpp *****

//*******************************************************************
//
//
// Description: This is a TCP socket program that illustrates how a relay server
//              take one message from a client and then sends the message to the
//              destination server and then receive a message from the destination
//              server and echo back to the original client
//
//  Input:      from client via TCP connection to a relay server,
//              and from relay server via TCP connection to a server,
//              and echo back to relay server and then echo back to the original client
//
//  Output:     message from client is written to the screen and message from destination server
//              is written to the screen
//
//  
//
//*******************************************************************
    // ***** tcprelaysocket1 *****
    
    
                        // header files

#include <stdio.h>      //for printf(), ...
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
using std::cin;

main( int argc, char *argv[])
{
   //creation of relay socket1 for receiving message from the client
   //Here the relay socket1 acts as a server socket
   int sock1fd, newsock1fd,          // file descriptor of sockets
       nread,                      // the # of bytes read 
       addrlen, client_addrlen;    // address length
   int my_port_id = MY_PORT_ID;
   struct sockaddr_in my_addr, client_addr;  // addresses for socket
   char msg[MESSAGESIZE];

   if (argc == 2)
   {
       my_port_id = atoi(argv[1]);
   }
   

   // ---------------------------------------------------------
   // Initialization:
   // ---------------------------------------------------------
    
   if ( (sock1fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
   { 
      cerr << "Server: socket error: " << errno << "\n"; 
      exit(1); 
   }

   // cout << "Server: constructing my local address\n";
   memset( &my_addr, 0, sizeof(my_addr));    // Zero out structure
   my_addr.sin_family = AF_INET;        // Internet address family
   my_addr.sin_addr.s_addr = htonl(INADDR_ANY);    // Any incoming interface 
   my_addr.sin_port = htons(my_port_id);    // my port


   // ---------------------------------------------------------
   // binding:
   // ---------------------------------------------------------
   if ( (bind(sock1fd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) )
   { 
      cerr << "Server: bind fail: " << errno << "\n"; 
      exit(2); 
   }   

   // ---------------------------------------------------------
   // Tell the OS for this socket to take message
   // ---------------------------------------------------------
   listen(sock1fd, SOMAXCONN);  //  decide by OS
   cout << "\n\nWaiting for client's message....\n\n";

   // Loop 
   client_addrlen = sizeof(client_addr);

   while(1)
   {
      // ---------------------------------------------------------
      // Wait for client's connection
      // ---------------------------------------------------------
      newsock1fd = accept(sock1fd, (struct sockaddr *) &client_addr, (socklen_t *)&client_addrlen);
      if (newsock1fd < 0)
      { 
         cerr << "Server: accept error: " << errno << "\n"; 
         exit(2); 
      }   

      cout << "Connection from client(" << inet_ntoa(client_addr.sin_addr);  //client IP
      cout << ":" << ntohs(client_addr.sin_port) << ")\n"; // client port 

      nread = read(newsock1fd,msg, MESSAGESIZE);
      // cout << "Server: retrun code from read is " << nread << "\n";
      if (nread >0) 
      {

         cout << "Client's message is: "; 
         cout.write(msg, nread);                    // Message from client
         cout << "\n";
      
      // ***** tcprelaysocket2 *****
       //creation of relay socket2 for sending message to the final/destination server
      //Here the relay socket2 acts as a client
      
            #define SERVER_PORT_ID  54713     
            
            #define SERV_HOST_ADDR "147.97.156.238" 
                        
            using std::cin;
            
               int sock2fd,     // socket descriptor
                   retcode;    // return code form some functions
               int server_port_id = SERVER_PORT_ID;
               char serv_host_addr[20]=SERV_HOST_ADDR;
               struct sockaddr_in server_addr;  // addresses for sockets
            
               // ---------------------------------------------------------
               // Initialization: 
               // ---------------------------------------------------------
               //cout << "Client: creating socket\n";
               //if ( (sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
               if ( (sock2fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
               { 
                   cerr << "Client: socket failed: "<< errno <<"....\n"; 
                   exit(1); 
               }
            
               // cout << "Client: constructing addr structure for server\n";
               memset( &server_addr, 0, sizeof(server_addr));  // Zero out structure
               server_addr.sin_family = AF_INET;        // Internet address family
               server_addr.sin_port = htons(server_port_id);// server port
               //server_addr.sin_addr.s_addr = inet_addr(serv_host_addr); // old style
               if (inet_aton(serv_host_addr, &(server_addr.sin_addr))==0) // get server addr
               {  // invalid server address
                   cerr << "Client: Invalid server address...\n";
                   exit(2); 
               }
               
            
               // ---------------------------------------------------------
               // Establish the connection 
               // ---------------------------------------------------------
               if (connect(sock2fd, (struct sockaddr *) &server_addr, sizeof(server_addr))<0)
               {
                   cerr << "Client: connect failed: "<< errno <<"....\n"; 
                   exit(3); 
               }
            
               // ---------------------------------------------------------
               // Message Preparation  
               // ---------------------------------------------------------
      
               cout << "getting and sending message from original client\n";
               //cin.getline(msg, MESSAGESIZE,'\n');
            
            
            
               // ---------------------------------------------------------
               // Transmission 
               // ---------------------------------------------------------
               retcode = write(sock2fd,msg,nread);
               if (retcode < 0)
               {
                   cerr << "client: send failed: " << errno << "....\n";
                   exit(4); 
               }   
            
               retcode = read(sock2fd,msg, MESSAGESIZE);
               // cout << "client: retrun code from read is " << nread << "\n";
               if (retcode >0)
               {
                  cout << "Server's message is: "; 
                  cout.write(msg, retcode);                    // Message from server
                  // cout << msg ;  // incorrect way to do
                  cout << "\n";
               }   
               // ---------------------------------------------------------
               // Termination 
               // ---------------------------------------------------------
               /* close socket */
               close(sock2fd);
      }

       
      // echo back
      //cin.get(msg, MESSAGESIZE);
      nread = write(newsock1fd,msg,nread);
     // done=true;
      if (nread < 0)
      {
         cerr << "server's send failed...\n";
         exit(3); 
      }   

      close(newsock1fd);
   }

   // ---------------------------------------------------------
   // Termination
   // ---------------------------------------------------------
   /* close socket */
   close(sock1fd);
   //-----------------------------------------------------------------

}

