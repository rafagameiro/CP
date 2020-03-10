CC=gcc
CFLAGS=-I. #only used if you want to load header files (.h)
FILE=approxPI.o

define check_file =
    if test -f "$(FILE)"; then
	rm approxPI.o    
    fi
endef


approxPImake: $(FILE)
	    @$(CC) -o approxPI $(FILE)



approxPI.o : approxPI.c
	    @$(CC) -c approxPI.c

clean :  
	@rm approxPI.o;
	@printf "Files removed.\n";
	
