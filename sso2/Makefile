.DEFAULT_GOAL := all
FLAGS = -Wall -DDEBUG -std=c99
all: clean pipes1.c
	gcc pipes1.c -o main $(FLAGS)
	# xterm -hold -e ./main 
milliontest: pipes1.c
	gcc pipes1.c -o main $(FLAGS) -DMillionTest
	./main -s -r #simple forking, for testing, do not read data
milliontesteagerClose: pipes1.c
	gcc pipes1.c -o main $(FLAGS) -DMillionTest
	./main -s -r -e #simple forking, for testing, do not read data, eager close
dontcloseOutput: pipes1.c
	gcc pipes1.c -o main $(FLAGS) -DDontCloseOutputOnChild
	./main -s #"simple...
DontCloseInputInParent: pipes1.c
	gcc pipes1.c -o main $(FLAGS) -DDontCloseInputInParent
	./main -s
runtest1:
	./main -phello
clean:
	rm ./main || echo already clean