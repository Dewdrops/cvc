PROGRAM := cvc
SOURCES := $(wildcard *.cpp)
OBJS    := $(patsubst %.cpp, %.o, $(SOURCES))

CC      := g++
CFLAGS  := -O2 -g -Wall
INCLUDE :=

PREFIX  := $(DESTDIR)/usr/local
BINDIR  := $(PREFIX)/bin

LIB     := -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_video

.PHONY: all clean install uninstall

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDE)

clean:
	rm -f $(PROGRAM) *.o

install: $(PROGRAM)
	@echo "Installing to $(BINDIR)"
	@mkdir -p $(BINDIR)
	@install -m 0755 $(PROGRAM) $(BINDIR) || \
		echo "Failed. Try "make PREFIX=~ install" ?"

uninstall:
	rm -rf $(BINDIR)/$(PROGRAM)

