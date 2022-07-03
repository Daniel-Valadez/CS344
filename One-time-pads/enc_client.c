//Enc_client file - some of this code is taken from the
//example program client.c that was provided.

#include "program5.h"

void setupAddressStruct(
    struct sockaddr_in * address,
    int portNumber,
    char * hostname
){
    memset((char *) address, '\0', sizeof(*address));
    address -> sin_family = AF_INET;
    address -> sin_port = htons(portNumber);

    struct hostent * hostInfo = gethostbyname(hostname);
    if(hostInfo == NULL){
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(1);
    }

    memcpy((char *) &address -> sin_addr.s_addr, hostInfo -> h_addr_list[0], hostInfo -> h_length);
}
int main(int argc, char *argv[]) {
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;

    char * file_buffer = NULL;
    char * key_buffer = NULL;
    char * buffer = NULL;
    long file_length;
    long key_length;
    long buffer_length;
    FILE *file_read;
    FILE *key_read;

    //Open the input file
    file_read = fopen(argv[1], "r");
    if(file_read){
        fseek (file_read, 0, SEEK_END);
        file_length = ftell(file_read);
        fseek (file_read, 0, SEEK_SET);
        file_buffer = malloc(file_length + 1);
        fread (file_buffer, 1, file_length, file_read);
        fclose (file_read);
    }

    //Remove the new line char at the end of the string and dec the file length by 1
    char* new_line = strstr(file_buffer, "\n");
    *new_line = '\0';
    file_length = file_length - 1;

    //For loop goes through all of the input characters and verifies they aren't bad
    for(int i = 0; i < file_length; ++i){
        int check_int = file_buffer[i] - 65;

        //If the check int is below 0 and not -33 (space) then it is bad
        if(check_int < 0 && check_int != -33){
            fprintf(stderr, "enc_client error: input contains bad characters\n");
            exit(1);
        }
        else if(check_int > 25){
            fprintf(stderr, "enc_client error: input contains bad characters\n");
            exit(1);
        }
    }

    //Same code as above, open the key file and read it in
    key_read = fopen(argv[2], "r");
    if(key_read) {
        fseek (key_read, 0, SEEK_END);
        key_length = ftell(key_read);
        fseek (key_read, 0, SEEK_SET);
        key_buffer = malloc(key_length + 1);
        fread (key_buffer, 1, key_length, key_read);
        fclose (key_read);
    }

    char* new_line_k = strstr(key_buffer, "\n");
    *new_line_k = '\0';
    key_length = key_length - 1;

    //Check to see if the key is at least the length of the file
    if(key_length < file_length){
        fprintf(stderr, "Error: key ‘%s’ is too short\n", argv[2]);
        exit(1);
    }

    buffer_length = (file_length * 2) + 5;
    buffer = malloc((file_length * 2) + 6);

    strcpy(buffer, "ENC");
    strcat(buffer, file_buffer);
    strncat(buffer, key_buffer, file_length);

    //Finally add on the terminating chars
    strcat(buffer, "@@");

    // Check usage & args - that there are enough arguments
    if (argc < 2) {
        fprintf(stderr, "USAGE: %s hostname port\n", argv[0]);
        exit(1);
    }

    //Create a socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0){
        fprintf(stderr, "CLIENT: ERROR opening socket");
        exit(1);
    }
    setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

    //Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
        fprintf(stderr, "CLIENT: ERROR connecting");
        exit(1);
    }

    //Write to the server checking to make sure all the chars were written
    charsWritten = send(socketFD, buffer, strlen(buffer), 0);
    if (charsWritten < 0){
        fprintf(stderr, "CLIENT: ERROR writing to socket");
    }
    if (charsWritten < strlen(buffer)){
        fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
    }

    //Set buffer to null terminated chars, creates a socket buffer to read in chunks
    memset(buffer, '\0', buffer_length);
    char socket_buffer[1000];

    //Read the server's response from the socket using a loop until
    //The terminating chars are found
    while (strstr(buffer, "@@") == NULL){
        if (strstr(buffer, "~~") != NULL) {
            fprintf(stderr, "Error: could not contact enc_server on port %s\n", argv[3]);
            exit(2);
        }
        memset(socket_buffer, '\0', 1000);

        //Read in the chunk from the socket
        charsRead = recv(socketFD, socket_buffer, sizeof(socket_buffer) - 1, 0);
        if (charsRead < 0){
            fprintf(stderr, "ERROR reading from socket\n");
        }

        //Add the chunk to the main buffer message
        strcat(buffer, socket_buffer);
    }

    //Remove the terminating characters
    char* terminalLoc = strstr(buffer, "@@");
    *terminalLoc = '\0';
    strcat(buffer, "\n");
    fprintf(stdout,"%s", buffer);
    close(socketFD);
    return 0;
}
