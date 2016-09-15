// ***** relaywebserver.cpp *****

//*******************************************************************
//
//
// Description: This is a TCP socket program that illustrates how a HTTP relay server
//              take one request from a browser and then sends the request to the
//              destination server and then receive a response from the destination
//              server and echo back to the original browser.
//
//  Input:      request from browser via TCP connection to a relay web server,
//              and from relay web server via TCP connection to a server,
//              and echo back to relay server and then echo back to the browser
//
//  Output:     request from browser is written to the screen and reply from destination web server
//              is written to the screen
//
//
//*******************************************************************
    
   
    
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
#include <fstream>      //for file operations

using std::cout;
using std::cerr;
using std::cin;
using std::ifstream;
using std::ofstream;
using std::fstream;

// ***** relaywebsocket2 *****
#define SERVER_PORT_ID  54713     
#define SERV_HOST_ADDR "147.97.156.237"                         
   int sock2fd,     // socket descriptor
   retcode;    // return code form some functions
   int server_port_id = SERVER_PORT_ID;
   char serv_host_addr[20]=SERV_HOST_ADDR;
   struct sockaddr_in server_addr;  // addresses for sockets
   //int sum=115;
   int sum=0;
   int retcode1=115;
   char *length;
   char *length1;
   int length2;

// ***** relaywebsocket1 *****
#define MY_PORT_ID   44713      
#define MESSAGESIZE  256      //buffer size

main( int argc, char *argv[])
{
   //creation of relay socket1 for receiving message from the browser
   //Here the relay socket1 acts as a server socket
   int sock1fd, newsock1fd,          // file descriptor of sockets
       nread,                      // the # of bytes read 
       addrlen, client_addrlen;    // address length
   int my_port_id = MY_PORT_ID;
   struct sockaddr_in my_addr, client_addr;  // addresses for socket
   char msg[MESSAGESIZE];
   char msg1[MESSAGESIZE];
   char msg2[5000000];
   char msg3[5000] = {0};
   char content[MESSAGESIZE];
   char x[20];
   int content_length;
   

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
   cout << "\n\nWaiting for client's request....\n\n";

   // Loop 
   client_addrlen = sizeof(client_addr);
   
   //while(1)
   //{

   // ---------------------------------------------------------
   // Wait for client's browser connection
   // ---------------------------------------------------------
   newsock1fd = accept(sock1fd, (struct sockaddr *) &client_addr, (socklen_t *)&client_addrlen);
   if (newsock1fd < 0)
   { 
      cerr << "Server: accept error: " << errno << "\n"; 
      exit(2); 
   }   

   cout << "Connection from client(" << inet_ntoa(client_addr.sin_addr);  //client IP
   cout << ":" << ntohs(client_addr.sin_port) << ")\n"; // client port 
      
   // ***** relaywebsocket2 *****
   //creation of relay socket2 for sending request to the final/destination server
   //Here the relay socket2 acts as a client browser
      
   // ---------------------------------------------------------
   // Initialization: 
   // ---------------------------------------------------------
   //cout << "Client: creating socket\n";
   //if ( (sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
   if ( (sock2fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
   { 
		cerr << "Relay: socket failed: "<< errno <<"....\n"; 
        exit(1); 
   }
            
   // cout << "Client: constructing addr structure for server\n";
   memset( &server_addr, 0, sizeof(server_addr));  // Zero out structure
   server_addr.sin_family = AF_INET;        // Internet address family
   server_addr.sin_port = htons(server_port_id);// server port
   server_addr.sin_addr.s_addr = inet_addr(serv_host_addr); // old style
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

   // ***** relaywebsocket1 read method *****
   while(1)
   {
	   nread = read(newsock1fd,msg, MESSAGESIZE);
	   // cout << "Server: retrun code from read is " << nread << "\n";
	   if (nread >0) 
	   {
		  cout << "Client's request is: "; 
		  cout.write(msg, nread);                    // Message from client browser
		  cout << "\n";
		  strncat(msg3,msg,nread);
	   }
	   
	   // ***** relaywebsocket2 transmission and termination *****
	   // ---------------------------------------------------------
	   // Message Preparation  
	   // ---------------------------------------------------------
	   // cout << "message got from browser to proxy, and that request is being sent from proxy to main server \n";
	   //cin.getline(msg, MESSAGESIZE,'\n');
	   
	   // ---------------------------------------------------------
	   // Transmission 
	   // ---------------------------------------------------------
	   //while((retcode = write(sock2fd,msg,nread))>0)
	   //{
	   retcode = write(sock2fd,msg,MESSAGESIZE);		//writing the message to the socket 2 that is from relay server to the server
	   if (retcode < 0)
	   {
		 cerr << "client: send failed: " << errno << "....\n";
		 exit(4); 
	   }
		  
	   if(strstr(msg3,"\r\n\r\n"))
	   {
		   cout<< "end of request str found\n";
		   break;
	   }
	   else
	   {
		  cout<< "The string does not exist \n"; 
	   }
	}
	   
 
	ofstream outfile;
	outfile.open("cached_file.txt");
	if (outfile)
	{
		outfile.close();	//if the file already exists then the file will be cleared and closed
	}
	  
   //while(sum<retcode1)
	while(sum<(content_length+115))
	{
		 retcode = read(sock2fd,msg, MESSAGESIZE);		//reading the message from the socket 2 that is from the server
		 //retcode1+=retcode;
		 // cout << "client: retrun code from read is " << nread << "\n";
		 if (retcode >0)
		{
		   cout<<"\n";
		   cout<<"\n This message is from the main server to the proxy server \n";
		   cout<<"\n";
		   ofstream outfile;    //file declaration
		   outfile.open("cached_file.txt", fstream::app);      //opening/binding a file            
		   cout << "Server's message is: "; 
		   cout.write(msg, retcode);                    // Message from server
		   outfile.write(msg, retcode);     //writing/caching the file to the local server 
		   // cout << msg ;  // incorrect way to do
		   cout << "\n"; 
		   strncat(msg2,msg,retcode);
		   sum+=retcode;
		   cout<<"\n The value of sum is:  "<<sum;
		} 
		/*if(strstr(msg2,"</html>") || strstr(msg2,"</HTML>") )
		{
			cout<<"\n end of html found\n";
			// break;
		}
		else
		{
			cout<<"\n end of html not found\n";
		}
	   
		if(strstr(msg2,"Content-Length: "))
		{
			cout<<"\n content length found\n";
		}
		else
		{
			cout<<"\n content length not found\n";
		}*/
		   
		if((strstr(msg,"Content-Length: ")))
		{
		   length = (strstr(msg,"Content-Length: "));
		   length = length + 16;
		   strcpy(content,length);
		   //cout<<"\n the content buffer is: "<<content;
		   length1 = (strstr(content,"\r\n"));
		   //cout<<"\n Length1 is: "<< length1;
		   //strcat(length2,length1);
		   //cout<<"\n the length 2 is: "<<length2;
		   //length2= (int)(length1)-(int)(length);
		   //cout<<"\n length2:   " <<length2;
		   //cout<<"\nlength1 value is  "<<(unsigned long)length1;
		   //cout<<"\nlength value is  "<<(unsigned long)length;
		   int i=0;
		   while(length<length1)
			{
				x[i]=length[0];
				length++;
				i++;
			}
		   
			cout<<"\n value of x is   "<<x;
		   
			content_length = atoi(x);
			cout<<"\n The content length for the data from the server is:  "<<content_length;
		   
			//if(strstr(content,"\r\n\r\n"))
			//{
			
			//}
			//cout<<"\n content - length is: " <<length;	   
		}
		   
		//cout<< "\n message still reading in while loop\n";
	}	
	outfile.close();     //closing the file
	  
	//cout<<"\n echo back from server \n";
	//cout<<"\n";
	ifstream infile;		//reading file declaration
	infile.open("cached_file.txt");			//opening/binding a file
	if(infile)
	{
		bool header_change = true;
		char temp[12];
		char temp1[MESSAGESIZE];
		int len =0;
		
		strcpy(temp,"RelayServer");
		while(!infile.eof())
		{
			memset(msg1,0,sizeof(msg1));
			len =0;
			if(header_change == true)
			{
				len = strlen(temp);
				
			}
			infile.read(msg1, MESSAGESIZE-len);		//reading the file from the local server 
				
			if(header_change == true)
			{
				strcpy(temp1,msg1);
				strcpy(&temp1[44],temp);
				strcat(temp1,&msg1[53]);
				strcpy(msg1,temp1);
				header_change = false;
				/* cout<<"i am printing \n\n\n\n\n\n";
				cout<<msg1<<"\n";
				cout<<"\n\n\n\n\n\n\n\n"; */
			}
				
			nread = write(newsock1fd,msg1,MESSAGESIZE);
			if (nread < 0)
			{
			   cerr << "Relay server's send failed...\n";
			   exit(3); 
			}
		}
	}
	else
	{
		cout<<"\n The file does not exist \n";
	}
   
	infile.close();
	//nread = write(newsock1fd,msg,retcode);	//sending the file read from the local relay server to the browser
	// done=true;
	/*if (nread < 0)
	{
	cerr << "Relay server's send failed...\n";
	exit(3); 
	}*/
            
	// ---------------------------------------------------------
	// Termination 
	// ---------------------------------------------------------
	/* close socket */
	//cout<<"\n closing sock2fd \n";
	//cout<<"\n this socket belongs to the proxy server - main server \n";         
	close(sock2fd);      
	//cout<<"\n closing newsock1fd \n ";
	//cout<<"\n this socket belongs to the browser - proxy server \n";
	close(newsock1fd);
   
	//}
	// ---------------------------------------------------------
	// Termination
	// ---------------------------------------------------------
	/* close socket */
	//cout<<"\n closing sock1fd \n";
	//cout<<"\n this socket belongs to the browser - proxy server \n";
	close(sock1fd);
	//-----------------------------------------------------------------
	cout<<"\n See the file cached_file.txt to see the downloaded file \n";
	
}

