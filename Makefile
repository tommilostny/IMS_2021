# IMS project 2021 (Silicon manufacturing process) Makefile.
# Authors: Tomáš Milostný (xmilos02)
#          Michal Rivola (xrivol01)
 
CC = g++
CXXFLAGS = -g -O2 -I/usr/local/lib
CXXLIBS = -lsimlib -lm
OBJS = main.o

# Compile mytftpclient and its dependencies.
main: $(OBJS)
	$(CC) $(CXXFLAGS) $^ -o $@ $(CXXLIBS)

run: main
	./$^

# Delete built files.
clean:
	rm -f *.o main xmilos02.tar

# Create .tar archive for project submission.
tar:
	tar -cf xmilos02.tar *.cpp *.hpp Makefile manual.pdf README.md
