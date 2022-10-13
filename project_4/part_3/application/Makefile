OBJS = pi_control_speed.o

all: pi_control_speed

pi_control_speed: $(OBJS)
	g++ -o pi_control_speed $(OBJS) -l pthread 

%.o: src/%.cpp
	g++ -o $@ -c $< -I include

clean:
	rm -f *.o pi_control_speed
	echo Clean done