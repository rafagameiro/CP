SRC=$(shell find src -name '*.java' -print)

all: $(SRC)
	mkdir -p bin
	javac -d bin $^

clean:
	rm -rf bin
