OBJS = brightness_master.o

all: brightness_master

brightness_master: $(OBJS)
	g++ -o brightness_master $(OBJS) -l pthread 

%.o: src/%.cpp
	g++ -o $@ -c $< -I include

clean:
	rm -f *.o brightness_master
	echo Clean done