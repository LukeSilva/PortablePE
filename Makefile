NAME=replace-head

all:
	g++ -o ${NAME} ${NAME}.cpp
	
clean:
	rm ${NAME}
