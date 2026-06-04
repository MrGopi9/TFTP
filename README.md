# TFTP
Designed to transfer files one system to another system which in the LAN by using UDP ( SOCK_DGRAM socket )


TFTP — Trivial File Transfer Protocol
What is TFTP?
TFTP is a simple file transfer protocol used to transfer files between a client and server over a network. It is called "trivial" because it is much simpler than FTP — no login, no directory listing, no complex commands.

Key Facts
FeatureDetailsFull formTrivial File Transfer ProtocolProtocolUDP (port 69)RFCRFC 1350AuthenticationNoneDirectionRead (RRQ) and Write (WRQ)Block size512 bytes per block

How it works
Two operations only:

RRQ — Read Request (client downloads a file from server)
WRQ — Write Request (client uploads a file to server)

Transfer flow:

Client sends RRQ or WRQ to server on port 69
Server picks a random port and responds
File is sent in 512-byte blocks
Receiver sends ACK for every block
If no ACK received → sender retransmits (timeout)
Last block has less than 512 bytes → signals end of file


Why UDP and not TCP?

TFTP is designed to be lightweight and fast
No connection setup overhead
TFTP handles its own reliability using ACK + retransmission
Perfect for small, simple file transfers on local networks


Where is TFTP used in real life?

Booting diskless systems — a device with no OS loads its kernel via TFTP from a server (PXE boot)
Network routers & switches — Cisco routers use TFTP to load or backup their configuration files
Embedded systems — firmware updates are pushed via TFTP
IP phones — download config files from a TFTP server at startup
IoT devices — lightweight protocol fits resource-constrained devices


TFTP vs FTP
FeatureTFTPFTPProtocolUDPTCPAuthenticationNoYes (username/password)CommandsOnly RRQ/WRQMany (ls, cd, get, put…)SpeedFast (simple)Slower (more overhead)SecurityNoneBasicUse caseBoot files, firmwareGeneral file transfer

Error codes in TFTP (from RFC 1350)
CodeMeaning0Not defined1File not found2Access violation3Disk full4Illegal TFTP operation5Unknown transfer ID6File already exists

Connection to your resume
You have already implemented a full TFTP server and client in C — which is a very strong project because:

You followed RFC 1350 from scratch
Handled RRQ and WRQ over UDP
Implemented block-by-block ACK
Handled retransmission on timeout
Detected last block (less than 512 bytes)





**Compilation process:**

gcc server.c -o tftp_server   ---> server file compilation
gcc client.c -o tftp_client   ---> client file compilation

Once compilation doine , launch the server and client app's.
First launch server app and then client.
while lanching server application, we need to provide PORT number 
while launching client , server_PORT number and server_IP require






Validated against tftp-hpa on Linux

This is not a beginner project — most developers never go this deep. It shows you can read and implement an RFC, which is a skill MNC companies look for in systems programmers. 💪
