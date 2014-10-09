all:
	mkdir -p build/
	gcc src/csa.c -o build/csa
	cp src/plot_script.gp build/
run:
	build/csa etc/SaludoAudacity.raw 2 0 1
clean:
	rm -rf build/
