OBJS = test_encoder.o

all: test_encoder

test_encoder: $(OBJS)
	g++ -o test_encoder $(OBJS) -l pthread 

%.o: src/%.cpp
	g++ -o $@ -c $< -I include

clean:
	rm -f *.o test_encoder
	echo Clean done