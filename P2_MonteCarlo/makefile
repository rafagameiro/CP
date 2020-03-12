
# target: source
# 	commands

#$^ -> refers to the source
#$@ -> refers to the target


CC=gcc
CFLAGS=-lpthread#-I. only used if you want to load header files (.h)

help:
	@printf "Commands:\n";
	@printf "\tseq - compile sequential program.\n";
	@printf "\tpar - compile parallel program.\n";

seq: approxPI.o
	    @$(CC) -o approxPI $^

par: approxPIPar.o
	    @$(CC) -o approxPIPar $^ $(CFLAGS)


%.o : %.c
	    @$(CC) -c $^

clean :  
	@rm approxPI.o;
	@rm approxPIPar.o;
	@printf "Files removed.\n";

remove: 
	@rm approxPI;
	@rm approxPIPar;
	@printf "Files removed.\n";
