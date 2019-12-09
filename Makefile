CCC=g++
CFLAGS=-g -I /usr/local/include/opencv4
LDFLAGS=-fPIC -Wl,-rpath,"/usr/local/lib64"

all: detecteur

test: traitement_image.o buffer.o 
	$(CCC) $(LDFLAGS) -o $@ $^ -lopencv_videoio -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc

%.o: %.cpp
	$(CCC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f test *.o
