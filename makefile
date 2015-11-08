cgic.cgi:cgic.o cgi_handle.o cJSON.o aria2c_control.o minidlnad_control.o client.o
	gcc cgi_handle.o cJSON.o aria2c_control.o minidlnad_control.o cgic.o client.o -lm -lpthread -o cgic.cgi
cgic.o:cgic.c
	gcc -c cgic.c
cgi_handle.o:cgi_handle.c cgi_handle.h
	gcc -c cgi_handle.c
cJSON.o:cJSON.c cJSON.h
	gcc -c cJSON.c
aria2c_control.o:aria2c_control.c
	gcc -c aria2c_control.c
minidlnad_control.o:minidlnad_control.c
	gcc -c minidlnad_control.c
client.o:client.c
	gcc -c client.c
clean:
	rm -f *~
	rm -f *.o
	rm -f *.cgi

