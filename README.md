# WindowsClient
#Commands for execution of server.c file

Windows

gcc -o server1.exe server1.c -lws2_32      # compiling server.c file on cmd

server1.exe                       # start the server or run the server

sc create SimpleTCPServer binPath= "C:\Users\s3502\Documents\virtual_box\server.exe"   # registering the server as windows service

sc start SimpleTCPServer     # start the service

sc stop SimpleTCPServer     # stop the service

cl /D _WIN32 server2.c ws2_32.lib Advapi32.lib  # compiling server2.c file on visual studio 2022 developer command prompt

Linux

gcc -o server1 server1.c     # compiling server.c file on Linux terminal

 ./server1             # start the daemon or run the daemon

ps aux | grep server1   #check wether daemon is running or not

kill <pid>   # to stop the daemon execution
