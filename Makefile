# IMS project 2021 (Silicon manufacturing process) Makefile.
# Authors: Tomáš Milostný (xmilos02)
#          Michal Rivola (xrivol01)
 
CC = g++
CXXFLAGS = -g -O2 -I/usr/local/lib
CXXLIBS = -lsimlib -lm
OBJS = example1.o

# Compile mytftpclient and its dependencies.
example1: $(OBJS)
	$(CC) $(CXXFLAGS) $^ -o $@ $(CXXLIBS)

run: example1
	./$^

# Delete built files.
clean:
	rm -f *.o example1 xmilos02.tar

# Create .tar archive for project submission.
tar:
	tar -cf xmilos02.tar *.cpp *.hpp Makefile manual.pdf README.md
