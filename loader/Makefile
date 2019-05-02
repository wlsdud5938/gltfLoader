HEADERS = json.hpp stb_image.h stb_image_write.h tiny_gltf.h
SOURCES = gltfLoader.cpp
CC = g++
CFLAGS = -std=c++11
LDFLAGS = -lGL -lGLEW -lglfw
EXECUTABLE = gltfLoader
RM = rm -rf

all: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES) 

clean:
	$(RM) *.o $(EXECUTABLE)