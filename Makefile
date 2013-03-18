all:
	gcc serv.c -o serv
	gcc client.c -o getfile

serv:
	gcc serv.c -o serv

client:
	gcc client.c -o getfile

clean:
	rm -f serv getfile
