OBJS = client_code.o

all: client_code

client_code: $(OBJS)
	g++ -o client_code $(OBJS) -l pthread 

%.o: src/%.cpp
	g++ -o $@ -c $< -I include

clean:
	rm -f *.o client_code
	echo Clean done