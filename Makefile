
all: test

test: test.o
	g++ -fPIC -o test test.o -lopencv_videoio -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc

test.o: test.cpp
	g++ -o test.o -I/usr/local/include/opencv4 -c test.cpp

clean:
	rm -f test test.o	
