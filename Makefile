CXX=clang++ -std=c++11 -stdlib=libc++
CPP_FILES:=$(wildcard src/*.cpp)
OBJ_FILES=$(patsubst src/%.cpp,%.o,$(CPP_FILES))
INCLUDES=-I/usr/local/include/SDL2
LIBS=-L/usr/local/lib -lm -lSDL2 -lSDL2_ttf -lSDL2_image

%.o : src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

all: $(OBJ_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LIBS) $^ -o raytracer