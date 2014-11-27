all : asd

asd : asd.cc test.cc
	g++ -O2 -D _SUNOS asd.cc test.cc /materialy/AISDI/hash/timer.cc -o asd 
	
del :
	rm asd
	rm asd3
debug :
	g++ -g -D _SUNOS asd.cc test.cc /materialy/AISDI/hash/timer.cc -o asd_debug 
	gdb asd_debug

view:
	lynx /materialy/AISDI/hash/info/index.html
