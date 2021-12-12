# IMS project 2021 (Silicon manufacturing process) Makefile.
# Authors: Tomáš Milostný (xmilos02)
#          Michal Rivola (xrivol01)
 
CC = g++
CXXFLAGS = -g -O2 -I/usr/local/lib
CXXLIBS = -lsimlib -lm
OBJS = main.o

# Compile mytftpclient and its dependencies.
chipshortage: $(OBJS)
	$(CC) $(CXXFLAGS) $^ -o $@ $(CXXLIBS)

run: chipshortage
	./$^
	gnuplot plot.gnuplot

# Delete built files.
clean:
	rm -f *.o chipshortage 02_xmilos02_xrivol01.zip chipshortage.png chipshortage.txt

# Create .tar archive for project submission.
zip:
	zip 02_xmilos02_xrivol01.zip main.cpp Makefile dokumentace.pdf plot.gnuplot
