# HTTP-1.1-Server-with-Backdoor-in-C
• Listened for incoming connections to the given port, and responded to most requests with a valid HTTP 1.1 response. • Executed the command when receiving a GET request for a URL in the form of /exec/&lt;command>, and returned an HTTP response with STDOUT of the executed command.

Description: 
Here I use c work on Ubuntu 14.04 64-bit to implement this assignment.
This program is a HTTP 1.1 sever that support valid HTTP 1.1 requests from HTTP clients. If server receives a GET request for a URL in the form of /exec/<command>, then my server would take <command> and execute it using the “system” and send the result of the stdout of the executed command back to clients.

Requirements:
Requires to be worked on Ubuntu 14.04 64-bit

Interface:
Must implement the following command-line interface:
./normal_web_server <port>
