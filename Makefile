EXEImglist = testimglist
OBJS  = PNG.o RGBAPixel.o lodepng.o testimglist.o imglist.o imglist-given.o

CXX = clang++
CXXFLAGS = -std=c++1y -c -g -O0 -Wall -Wextra -pedantic
LD = clang++
LDFLAGS = -std=c++1y -lpthread -lm

all : testimglist

$(EXEImglist) : $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXEImglist)

testimglist.o : testimglist.cpp imglist.h imglist-private.h cs221util/PNG.h cs221util/RGBAPixel.h
	$(CXX) $(CXXFLAGS) testimglist.cpp

imglist.o : imglist.cpp imglist.h imglist-private.h
	$(CXX) $(CXXFLAGS) -Wfloat-conversion imglist.cpp

imglist-given.o : imglist-given.cpp imglist.h
	$(CXX) $(CXXFLAGS) -Wfloat-conversion imglist-given.cpp

PNG.o : cs221util/PNG.cpp cs221util/PNG.h cs221util/RGBAPixel.h cs221util/lodepng/lodepng.h
	$(CXX) $(CXXFLAGS) cs221util/PNG.cpp

RGBAPixel.o : cs221util/RGBAPixel.cpp cs221util/RGBAPixel.h
	$(CXX) $(CXXFLAGS) cs221util/RGBAPixel.cpp

lodepng.o : cs221util/lodepng/lodepng.cpp cs221util/lodepng/lodepng.h
	$(CXX) $(CXXFLAGS) cs221util/lodepng/lodepng.cpp

clean :
	-rm -f *.o $(EXEImglist)
