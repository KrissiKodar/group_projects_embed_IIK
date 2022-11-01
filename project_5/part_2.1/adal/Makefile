OBJS = adal_code.o

all: adal_code

adal_code: $(OBJS)
	g++ -o adal_code $(OBJS) -l pthread 

%.o: src/%.cpp
	g++ -o $@ -c $< -I include

clean:
	rm -f *.o adal_code
	echo Clean done