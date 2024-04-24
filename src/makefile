compile: main.cpp
	if [ -d ../bin ]; then rm -rf ../bin; fi
	mkdir ../bin
	g++ -o ../bin/main main.cpp -lzmq -lcrypto

clean:
	rm -f ../bin/main