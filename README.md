A simple FTP client-server in C

Compile:
```
gcc -o client -g sftp_client.c
gcc -o server -g sftp_server.c
```

Usage:
1) Start server (default listen on port 1337)
```c
./server
```

2) Client
```c
./client <input_filename> <output_filename> <server_ip_address> <server_port#>
```
