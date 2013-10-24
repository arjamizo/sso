.DEFAULT_GOAL := all
FLAGS = -Wall -DDEBUG -std=c99
all: clean fifos1.c
	gcc fifos1.c -o main $(FLAGS)
	# xterm -hold -e ./main 
milliontest: fifos1.c
	gcc fifos1.c -o main $(FLAGS) -DMillionTest
	./main -s -r #simple forking, for testing, do not read data
milliontesteagerClose: fifos1.c
	gcc fifos1.c -o main $(FLAGS) -DMillionTest
	./main -s -r -e #simple forking, for testing, do not read data, eager close
dontcloseOutput: fifos1.c
	gcc fifos1.c -o main $(FLAGS) -DDontCloseOutputOnChild
	./main -s #"simple...
DontCloseInputInParent: fifos1.c
	gcc fifos1.c -o main $(FLAGS) -DDontCloseInputInParent
	./main -s
runtest1:
	./main -phello
clean:
	rm ./main || echo already clean