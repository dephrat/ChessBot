CXX = g++
CXXFLAGS = -std=c++14 -g -Wall -MMD -Werror=vla
OBJECTS = piece.o display.o chessmodule.o server.o client.o main.o chessexception.o subject.o chessstrategy.o level1.o level2.o level3.o level4.o window.o graphicsdisplay.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = chess

${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC} -lX11

clean :
	rm ${DEPENDS} ${OBJECTS} ${EXEC}

-include ${DEPENDS} # reads the .d files and reruns dependencies
