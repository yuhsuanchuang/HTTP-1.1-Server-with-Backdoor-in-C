# HTTP-1.1-Server-with-Backdoor-in-C
• Listened for incoming connections to the given port, and responded to most requests with a valid HTTP 1.1 response. • Executed the command when receiving a GET request for a URL in the form of /exec/&lt;command>, and returned an HTTP response with STDOUT of the executed command.

Requirements:
Requires to be worked on Ubuntu 14.04 64-bit

Interface:
Must implement the following command-line interface:
./normal_web_server &lt;port>
