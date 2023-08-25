all: nuntium-client

run_ip = 0.0.0.0
run_port = 8080
run: nuntium-client
	./nuntium-client.out "$(run_ip)" $(run_port)

nuntium-client:
	cc -lpthread src/*.c -o nuntium-client.out

clean:
	rm nuntium-client.out