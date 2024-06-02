# WindowsClient
#Commands for execution of server.c file

Windows

gcc -o server.exe server.c -lws2_32      # compiling server.c file on cmd

server.exe                       # start the server or run the server

sc create SimpleTCPServer binPath= "C:\Users\s3502\Documents\virtual_box\server.exe"   # registering the server as windows service

sc start SimpleTCPServer     # start the service

sc stop SimpleTCPServer     # stop the service

Linux

gcc -o server server.c     # compiling server.c file on Linux terminal

 ./server             # start the daemon or run the daemon
