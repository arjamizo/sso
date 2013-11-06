.DEFAULT_GOAL := all
FLAGS = -Wall -DDEBUG -std=c99
# all: clean pipes1.c
# 	gcc pipes1.c -o main $(FLAGS)
# 	# xterm -hold -e ./main 
bin/%: %.c
	@pwd
	@echo Building target bin/$@, first dependency $<
	mkdir -p bin
	gcc $< -o "$@" $(FLAGS)
# runtest1:
# 	./main -phello
clean:
 	rm -r ./bin || echo already clean