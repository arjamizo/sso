.DEFAULT_GOAL := all
FLAGS = -Wall -DDEBUG -std=gnu99
LIBS = -lrt
.PHONY := run
# all: clean pipes1.c
# 	gcc pipes1.c -o main $(FLAGS)
bin/%: %.c
	@pwd
	@echo Building target bin/$@, first dependency $<
	mkdir -p bin
	gcc $< -o "$@" $(FLAGS) $(LIBS)
# runtest1:
# 	./main -phello
clean:
	rm -r ./bin/* || echo already clean
