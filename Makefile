CFLAGS = -g -Wall

objects = socklib.o EZFileServer.o splitline.o
group2 = socklib.o c_tcp_client.o

EZFileServer : $(objects)
				gcc -Wall -Wextra -g -o EZFileServer $(objects)

c_tcp_client : socklib.o c_tcp_client.o
				gcc -Wall -Wextra -g -o c_tcp_client $(group2)

.PHONEY : clean
clean :
		-mv EZFileServer $(objects) c_tcp_client c_tcp_client.o /tmp
