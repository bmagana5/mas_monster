CFLAGS = -I ./include
##LIB    = ./lib/fmod/libfmodex64.so
DEVSOURCE = brianM.cpp krystalR.cpp graceloveS.cpp angelaT.cpp
WFLAGS = -Wall -Wextra
LFLAGS = -lrt -lX11 -lGLU -lGL -lm #-lXrandr
SSLFLAGS = -lcrypto -lssl
AUDIO = -D USE_OPENAL_SOUND \
	/usr/lib/x86_64-linux-gnu/libopenal.so \
	/usr/lib/x86_64-linux-gnu/libalut.so

all: rainforest


rainforest: rainforest.cpp log.cpp $(DEVSOURCE) credits.cpp 
		g++ $(CFLAGS) rainforest.cpp $(DEVSOURCE) log.cpp credits.cpp \
		libggfonts.a $(WFLAGS) $(SSLFLAGS) $(LFLAGS) \
		-o rainforest

clean:
	rm -f rainforest x.x
	rm -f *.o

