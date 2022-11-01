OBJS = master_code.o

all: master_code

master_code: $(OBJS)
	g++ -o master_code $(OBJS) -l pthread 

%.o: src/%.cpp
	g++ -o $@ -c $< -I include

clean:
	rm -f *.o master_code
	echo Clean done