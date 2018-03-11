PROJECT DESCRIPTION AND DESIGN:

Project is based on webproxy server program
The proxy accepts the incoming client http  GET request and parsers the client request.Reconstruct Http GET request with connection as close and accept as text/html. Requests based on the absolute URL sends it to the server .



eg: if client sends the request as GET http://www.google.com HTTP/1.0
web proxy rebuilds the request as

GET /index.html HTTP/1.1
Host: www.example.com
Accept-Language: en-us
Accept:text/html
Connection: Close


TEAM MEMBERS AND CONTRBUTION:
Rutu    :: Testing and validating all cases,Testing against all the websites , Debugging the errors
Shivani :: Parsing client request and reconstructing HTTP request, code clean up, Debugging the errors
Sowmya  :: Implementing proxy client ,server connections and socket communication, Pthread functionality 
 


HOW TO COMPILE:
Open the terminal
Go to the project folder 
Run the command 'make'

HOW TO RUN THE  PROXY :
Open the terminal 
Go to the folder in which server executable is present 
Run the command "$./proxy" <port#> < to start the proxy server
try requesting a page using telnet
telnet localhost <port#> 
Trying 127.0.0.1...
Connected to localhost.localdomain (127.0.0.1).
Escape character is '^]'.
GET http://www.cnn.com/ HTTP/1.0

Complex Testing
configure Firefox to use your proxy server as its web proxy as follows
  1.Go to the 'Edit' menu. 
  2.Select 'Preferences'. Select 'Advanced' and then select 'Network'.
  3.Under 'Connection', select 'Settings...'.  
  4.Select 'Manual Proxy Configuration'. Enter the hostname and port where your proxy program is running.
  5.Save your changes by selecting 'OK' in the connection tab and then select 'Close' in the preferences tab. 

Because Firefox defaults to using HTTP/1.1 and your proxy speaks HTTP/1.0
1.Type 'about:config' in the title bar.
2.In the search/filter bar, type 'network.http.proxy'
3.You should see three keys: network.http.proxy.keepalive, network.http.proxy.pipelining, and network.http.proxy.version.4.Set keepalive to false. Set version to 1.0. Make sure that pipelining is set to false.


More Details:
ABOUT HTTP Proxy
HTTP ProxyOrdinarily, HTTP is a client-server protocol. The client (usually your web browser) communicates directly with  the  server  (the  web  server  software).  However,  in  some  circumstances  it  may  be  useful  to  introduce an intermediate entity called a proxy. Conceptually, the proxy sits between the client and the server.  In  the  simplest  case,  instead  of  sending  requests  directly  to  the  server  the  client  sends  all  its requests  to  the proxy.  The  proxy  then  opens  a  connection  to  the  server,  and  passes  on  the  client's  request.  The  proxy  receives  the  reply  from  the  server,  and  then  sends  that  reply  back  to  the  client.Notice  that  the  proxy  is  essentially  acting  like  both  a  HTTP  client  (to  the  remote  server)  and  a  HTTP  server (to the initial client).
  



