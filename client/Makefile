all: nuntium-client

run: nuntium-client
	./nuntium-client.out

nuntium-client:
	cc -lpthread src/*.c -o nuntium-client.out

clean:
	rm nuntium-client.out