CFLAGS =  -ansi -Wall -Wextra -Wshadow -Wcast-align -Wunused -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion

#CFLAGS += -fsanitize=address,leak
#CFLAGS += -fsanitize=thread
#CFLAGS += -fsanitize=memory

# debug setting
CFLAGS += -O0 -g

# perf setting
# CFLAGS += -O3 -flto -DNDEBUG

all: project

project: project.c

fmt:
	@clang-format -i -style=file project.c

tidy:
	@clang-tidy project.c

test: project
	@sbin/test.sh

clean:
	@rm -f project project.o out.txt