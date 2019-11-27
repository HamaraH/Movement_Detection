
all: test

test: test.o
	g++ -o test test.o -lopencv_videoio -lopencv_core -lopencv_highgui

test.o: test.cpp
	g++ -o test.o -c test.cpp

clean:
	rm -f test test.o	
