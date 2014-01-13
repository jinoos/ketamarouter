#make clean && autoreconf -i && ./configure && make && make check
autoreconf -fi && ./configure && make && ./src/krouter
