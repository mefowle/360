CC=g++
CFLAGS=-Wall -g
LIBS=-lm 
DEPS=dns.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

dns: dns.o
	g++ -o dnsq dns.o $(CFLAGS) $(LIBS)
	
backup:
	rm -f dnsq.tar.gz *.o *~ ./dnsq
	tar -cf dnsq.tar *
	gzip -f dnsq.tar

clean:
	rm -f dnsq.tar.gz *.o *~ ./dnsq
