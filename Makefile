program = ./a.out
arg =
srcs = main.cpp glad.c

.PHONY: all
all: build run

.PHONY: build
build: $(program)

$(program): $(srcs)
	clang++ -std=c++17 -fcolor-diagnostics -fansi-escape-codes -Wall -Wno-deprecated -g \
		-I. -I./glad/include $(shell pkg-config --cflags glfw3 gl) \
		$(shell pkg-config --libs glfw3 gl) \
		-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreFoundation \
		$(srcs) \
		-o $(program)

.PHONY: clean
clean:
	rm -f $(program)

.PHONY: run
run:
	$(program) $(arg)
