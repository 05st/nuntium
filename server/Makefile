all: nuntium-server

run_port = 8080
run: nuntium-server
	./nuntium-server.out $(run_port)

nuntium-server:
	gcc src/*.c -o nuntium-server.out

clean:
	rm nuntium-server.out