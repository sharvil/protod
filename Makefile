PREFIX ?= /usr/local
CXXFLAGS += -Wall -Werror -std=c++11
EXTRA_FLAGS := `pkg-config --cflags --libs protobuf`

.PHONY: all clean install uninstall

all:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) src/protod.cpp -o protod

clean:
	rm -f protod

install: all
	cp protod $(PREFIX)/bin/

uninstall:
	rm -f $(PREFIX)/bin/protod
