main:
	g++ -g -Wall $$(pkg-config --cflags --libs /usr/local/Cellar/opencv/4.1.1_2/lib/pkgconfig/opencv4.pc) -std=c++11 carving.cpp -o carve

clean:
	rm -rf carve *.dSYM


