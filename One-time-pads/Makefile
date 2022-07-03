all: keygen enc_client enc_server dec_client dec_server

keygen: keygen.c
	gcc --std=gnu99 keygen.c -o keygen

enc_client:
	gcc --std=gnu99 enc_client.c -o enc_client

enc_server: 
	gcc --std=gnu99 enc_server.c -o enc_server

dec_client:
	gcc --std=gnu99 dec_client.c -o dec_client

dec_server:
	gcc --std=gnu99 dec_server.c -o dec_server

clean:
	rm keygen enc_client enc_server dec_client dec_server
