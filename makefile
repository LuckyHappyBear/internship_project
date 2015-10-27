aria2c_control:aria2c_control.o
	gcc aria2c_control.o -o aria2c_control
aria2c_control.o:aria2c_control.c
	gcc -c aria2c_control.c
clean:
	rm -f *~
	rm -f *.o
	rm -f *.sh
	rm -f *.txt
	rm -f aria2c_control
