all: nuntium

run_ip = 0.0.0.0
run_port = 8080
run: nuntium
	./nuntium.out "$(run_ip)" $(run_port)

nuntium:
	cc src/*.c -o nuntium.out -lncurses

clean:
	rm nuntium.out