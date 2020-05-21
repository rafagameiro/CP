SRC=$(shell find src -name '*.java' -print)

all: $(SRC)
	javac -d bin $^

clean:
	rm -rf bin
