
CFLAGS = -O -g
LIBS   = -lpng
CC     = g++

OBJ    = area.o  bezier.o  bitmap.o  main.o  parameter.o  shortest_path.o  sp_bezier.o  sp_lines.o  svg.o  tracer.o

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

spvec: $(OBJ)
	$(CC) $(CFLAGS) -o spvec $(OBJ) $(LIBS)

clean:
	rm $(OBJ) spvec

