IDIR=include
CC=clang++
CFLAGS=-I$(IDIR) -std=c++17 -O3

SDIR=src
ODIR=$(SDIR)/obj
LDIR=lib

LIBS=-lm -lpthread -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lstdc++fs

_DEPS = Event.hpp Camera.hpp Vec2.hpp Filesystem.hpp Hitreg.hpp Log.hpp Fonts.hpp Levels.hpp Menu.hpp Entities.hpp RocketJump.hpp Render.hpp Core.hpp Sound.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o Core.o Event.o Hitreg.o Vec2.o Levels.o Log.o Fonts.o Filesystem.o Render.o Sound.o RocketJump.o Camera.o Entities.o Menu.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

OUTPUT = AEROMIC

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

make: $(OBJ)
	$(CC) $(CFLAGS) -o $(OUTPUT) $^ $(LIBS)
	
debug: $(OBJ)
	$(CC) $(CFLAGS)-g -o $(OUTPUT) $^ $(LIBS) 

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
