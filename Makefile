all:
	mkdir -p build/
	gcc src/csa.c -o build/csa
run:
	build/csa etc/SaludoAudacity.raw 3 0 1 
clean:
	rm -rf build/