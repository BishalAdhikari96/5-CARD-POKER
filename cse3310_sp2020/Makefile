# this is an example makefile for the first homework
# assignment this semester

all: asio-1.12.2 src/chat_client src/chat_server

asio-1.12.2:
	tar xzf asio-1.12.2.tar.gz
	
GTKLINKFLAGS=$(shell pkg-config --libs gtkmm-3.0)
GTKCOMPILEFLAGS=$(shell pkg-config --cflags gtkmm-3.0)

CXXFLAGS+= -DASIO_STANDALONE -Wall -O0 -g -std=c++11
CPPFLAGS+= -I./include/ -I./asio-1.12.2/include -I./src 
LDLIBS+= -lpthread

src/chat_client: src/chat_client.cpp src/player.cpp
	${CXX} ${CXXFLAGS} ${CPPFLAGS} ${GTKCOMPILEFLAGS}  -o $@ $^  ${LDLIBS} ${GTKLINKFLAGS} `pkg-config --cflags --libs x11`

#chat_server.o: chat_server.cpp *.h
#	$(CXX) $(CXXFLAGS) -c chat_server.cpp chat_server.hpp class_deck.hpp $(LDLIBS) $(GTKFLAGS)
#chat_client.o: chat_client.cpp *.h
#	$(CXX) $(CXXFLAGS) -c chat_client.cpp chat_client.hpp class_message.hpp player.hpp class_deck.hpp $(GTKFLAGS) $(LDLIBS) 

  clean:
#	-rm -f src/player_client
	-rm -f src/chat_client
	-rm -f src/chat_server
#	-rm -f src/json		


