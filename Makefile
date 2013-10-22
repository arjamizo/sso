.DEFAULT_GOAL := milliontest
all: clean pipes1.c
	gcc pipes1.c -o main -Wall -DDEBUG -std=c99
milliontest: pipes1.c
	gcc pipes1.c -o main -Wall -DDEBUG -std=c99 -DMillionTest
	./main -s -r #simple forking, for testing, do not read data
milliontesteagerClose: pipes1.c
	gcc pipes1.c -o main -Wall -DDEBUG -std=c99 -DMillionTest
	./main -s -r -e #simple forking, for testing, do not read data, eager close
dontcloseOutput: pipes1.c
	gcc pipes1.c -o main -Wall -DDEBUG -std=c99 -DDontCloseOutputOnChild
	./main -s #"simple...
DontCloseInputInParent: pipes1.c
	gcc pipes1.c -o main -Wall -DDEBUG -std=c99 -DDontCloseInputInParent
	./main -s
runtest1:
	./main -phello
clean:
	rm ./main || echo already clean