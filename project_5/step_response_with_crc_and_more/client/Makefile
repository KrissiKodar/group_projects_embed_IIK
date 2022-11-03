OBJS = step_response.o

all: step_response

step_response: $(OBJS)
	g++ -o step_response $(OBJS) -l pthread 

%.o: src/%.cpp
	g++ -o $@ -c $< -I include

clean:
	rm -f *.o step_response
	echo Clean done