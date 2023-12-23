program = ./a.out
arg =
srcs = main.cpp glad/glad.c long_meiro.c

.PHONY: all
all: build run

.PHONY: build
build: $(program)

$(program): $(srcs)
	clang++ -std=c++17 -fcolor-diagnostics -Wall -Wno-deprecated -g \
		$(shell pkg-config --cflags glfw3 gl) \
		$(shell pkg-config --libs   glfw3 gl) \
		-I . \
		-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreFoundation \
		$(srcs) \
		-o $(program)

.PHONY: clean
clean:
	rm -f $(program)

.PHONY: run
run:
	$(program) $(arg)
