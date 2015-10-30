cgic.cgi:cgic.o cgi_handle.o cJSON.o aria2c_control.o minidlnad_control.o
	gcc cgi_handle.o cJSON.o aria2c_control.o minidlnad_control.o cgic.o -lm -o cgic.cgi
cgic.o:cgic.c
	gcc -c cgic.c
cgi_handle.o:cgi_handle.c cgi_handle.h
	gcc -c cgi_handle.c
cJSON.o:../cJSON/cJSON.c ../cJSON/cJSON.h
	gcc -c ../cJSON/cJSON.c
aria2c_control.o:../func_control_c/aria2c_control.c
	gcc -c ../func_control_c/aria2c_control.c
minidlnad_control.o:../func_control_c/minidlnad_control.c
	gcc -c ../func_control_c/minidlnad_control.c
clean:
	rm -f *~
	rm -f *.o
	rm -f *.cgi

