# IMS project 2021 (Silicon manufacturing process) Makefile.
# Authors: Tomáš Milostný (xmilos02)
#          Michal Rivola (xrivol01)
 
CC = g++
CXXFLAGS = -g -O2 -I/usr/local/lib
CXXLIBS = -lsimlib -lm
OBJS = main.o

chipshortage: $(OBJS)
	$(CC) $(CXXFLAGS) $^ -o $@ $(CXXLIBS)

run: chipshortage
	./$^
	gnuplot plot.gnuplot

clean:
	rm -f *.o chipshortage 02_xmilos02_xrivol01.zip chipshortage.png chipshortage.txt

zip:
	zip 02_xmilos02_xrivol01.zip main.cpp Makefile dokumentace.pdf plot.gnuplot
