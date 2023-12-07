INITIALIZING SERVER ZK

1 - Server starts with ZK's ip address and port plus it's own IP address and port
and number of lists (which have to be the same across all servers)

2 - Connect to ZK via zk_init

3 - Get IP from current tail if one exists and TCP connect to it

4 - copy the table via gettable ops

5 - (reset stats for new server?)

6 - register new server as tail in ZK

7 - Set old tail's next_server to the new server

8 - Connect old tail to new tail as a client

SERVER ACTING AS CLIENT

- Interpolate puts and deletes in table_skel.c:invoke with request to put into
next_server

SERVER'S WATCH CALLBACK

- Whenever CB is triggered, check whether znode for its next_server still exists,
otherwise close connection, update next_server, and connect to it

CLIENT'S WATCH CALLBACK

- Whenever CB is triggered, check wheter znodes for head and tail still exist,
otherwise close connections, update head and/or tail and connect to them
