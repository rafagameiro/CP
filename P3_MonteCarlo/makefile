
# target: source
# 	commands

#$^ -> refers to the source
#$@ -> refers to the target


CC=gcc
CFLAGS=-fopenmp#-I. only used if you want to load header files (.h)

help:
	@printf "Commands:\n";
        @printf "\thello - compile an hello program with OpenMP.\n"
	@printf "\tcarlo - compile monte carlo estimation program.\n";
        @printf "\tint - compile numerical integration estimation program.\n";

hello: hello.o
	    @$(CC) -o hello $^ $(CFLAGS)

carlo: carloPIPar.o
	    @$(CC) -o carloPIPar $^ $(CFLAGS)

int: intPIPar.o
	    @$(CC) -o intPIPar $^ $(CFLAGS)

%.o : %.c
	    @$(CC) -c $^

clean : 
         
        @rm hello.o;
	@rm carloPIPar.o;
        @rm intPIPar.o;
	@printf "Files removed.\n";

remove: 
	@rm hello;
        @rm carloPIPar;
        @rm intPIPar;
	@printf "Files removed.\n";
