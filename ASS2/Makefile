# Don't use this makefile, only exists in case of an automated testing script, check readme for instructions on how to really run

all: 
	$(MAKE) -C ./client
	$(MAKE) -C ./server
	cp ./client/pollSwayer ./pollSwayer
	cp ./server/poller ./poller

clean:
	$(MAKE) -C ./client clean
	$(MAKE) -C ./server clean
	rm -f ./pollSwayer
	rm -f ./poller