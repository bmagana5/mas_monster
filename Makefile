CFLAGS = -I ./src
##LIB    = ./lib/fmod/libfmodex64.so
DEVSOURCE = brianM.cpp krystalR.cpp graceloveS.cpp angelaT.cpp
SOURCE = src/Image.cpp src/Player.cpp src/Obstacle.cpp src/timers.cpp src/Global.cpp
HEADER = src/Image.h src/Player.h src/Obstacle.h src/timers.h src/Global.h src/Texture.h
WFLAGS = -Wall -Wextra
LFLAGS = -lrt -lX11 -lGLU -lGL -lm #-lXrandr
SSLFLAGS = -lcrypto -lssl
AUDIO = -D USE_OPENAL_SOUND \
	/usr/lib/x86_64-linux-gnu/libopenal.so \
	/usr/lib/x86_64-linux-gnu/libalut.so
COORD = -D COORD_TEST

all: monster test_coord

monster: monster.cpp log.cpp $(DEVSOURCE) credits.cpp $(SOURCE) $(HEADER)
		g++ $(CFLAGS) monster.cpp $(DEVSOURCE) $(SOURCE) log.cpp \
		credits.cpp libggfonts.a $(WFLAGS) $(SSLFLAGS) $(LFLAGS) \
		-o monster
test_coord: monster.cpp log.cpp $(DEVSOURC) credits.cpp $(SOURCE) $(HEADER)
	g++ $(CFLAGS) monster.cpp $(COORD) $(DEVSOURCE) $(SOURCE) log.cpp \
	credits.cpp libggfonts.a $(WFLAGS) $(SSLFLAGS) $(LFLAGS) \
	-o test_coord
clean:
	rm -f monster test_coord x.x
	rm -f *.o

