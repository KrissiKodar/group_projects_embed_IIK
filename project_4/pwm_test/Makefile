OBJS = test_pwm.o

all: test_pwm

test_pwm: $(OBJS)
	g++ -o test_pwm $(OBJS) -l pthread 

%.o: src/%.cpp
	g++ -o $@ -c $< -I include

clean:
	rm -f *.o test_pwm
	echo Clean done