//Dec_server.c

#include "program5.h"

void setupAddressStruct(struct sockaddr_in* address, int portNumber){
  memset((char*) address, '\0', sizeof(*address));
  address->sin_family = AF_INET;
  address->sin_port = htons(portNumber);
  address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char * argv[]){
    int connectionSocket, charsRead, charsWritten;
    char buffer[150000];
    char pipe_buffer[1000];
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);
    bool connect_bool = true;

    //Check usage & args
    if(argc < 2){
        fprintf(stderr,"USAGE: %s port\n", argv[0]);
        exit(1);
    }

    // Create the socket that will listen for connections
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(listenSocket < 0){
        fprintf(stderr, "ERROR opening socket");
        exit(1);
    }

    //Set up the address struct for the server socket
    setupAddressStruct(&serverAddress, atoi(argv[1]));

    //Associate the socket to the port
    if(bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
        fprintf(stderr, "ERROR on binding");
        exit(1);
    }

    //Start listening for connetions. Allow up to 5 connections to queue up
    listen(listenSocket, 5);

    //Accept a connection, blocking if one is not available until one connects
    while(1){

        //Accept the connection request which creates a connection socket
        connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
        if(connectionSocket < 0){
            fprintf(stderr, "ERROR on accept");
        }

        //Fork a child process
        pid_t childPid = fork();
        if(childPid == 0){

            //Set the buffer to null terminated chars, creates a socket buffer to
            //read in chunks, and sets beginning_read to true
            memset(buffer, '\0', 150000);
            char socket_buffer[1000];
            bool beginning_read = true;

            //Read the client's message from the socket using a loop until
            //the terminating chars are found
            while(strstr(buffer, "@@") == NULL){
                //First clear the socket buffer
                memset(socket_buffer, '\0', 1000);

                //Read in the chunk from the socket
                charsRead = recv(connectionSocket, socket_buffer, sizeof(socket_buffer) - 1, 0);
                if(charsRead < 0){
                    fprintf(stderr, "ERROR reading from socket\n");
                }

                //Checks to make sure that the connection is coming from the DEC client
                //First checks to see if beginning read is true
                if(beginning_read == true){
                    char check_source[4];
                    strncpy(check_source, socket_buffer, 3);
                    int cmp_enc = strcmp(check_source, "DEC");
                    if(cmp_enc != 0){
                        //If the first few chars aren't ENC, send an error message back to the
                        //client and close the connection by changing connect_bool
                        charsRead = send(connectionSocket, "~~ERROR: Source is Wrong", 22, 0);
                        if(charsRead < 0){
                            fprintf(stderr, "ERROR reading from socket\n");
                        }

                        //Set connect bool to false, this skips down to the end where the connection is closed
                        connect_bool = false;
                        break;
                    }
                    else{
                        beginning_read = false;
                    }
                }

                //Add the chunk to the main buffer message
                strcat(buffer, socket_buffer);
            }

            //Checks to make sure the connection is valid before proceeding
            if(connect_bool == true){

                //Remove the terminal indicator of "@@"
                char* terminal_buff = strstr(buffer, "@@");
                *terminal_buff = '\0';

                //Remove the first 3 chars of DEC from the string
                char input_string[strlen(buffer) - 2];
                memset(input_string, '\0', strlen(buffer) - 2);
                strcpy(input_string, &buffer[3]);

                //The received buffer string includes both the plain text and the key in one string
                //Since each piece is the same length, the length of each piece is half of the whole
                long input_length = strlen(input_string) / 2;

                //Create and null terminate a string for the plain text, key, and cypher
                char cypher_text[input_length + 1];
                memset(cypher_text, '\0', input_length + 1);
                char key[input_length + 1];
                memset(key, '\0', input_length + 1);
                char plain_text[input_length + 3];
                memset(plain_text, '\0', input_length + 3);

                //Copy the first half of the buffer to cypher text and second half to the key
                strncpy(cypher_text, input_string, input_length);
                strcpy(key, &input_string[input_length]);

                //Go through each character of the text file and convert it to
                //an int 0-26, do the same with the key

                for(int i = 0; i < input_length; ++i){
                    int text_int = cypher_text[i] - 65;
                    if(text_int == -33){
                        text_int = 26;
                    }
                    int key_int = key[i] - 65;
                    if(key_int == -33){
                        key_int = 26;
                    }

                    //Subtract the key from the cipher, and depending on if the result is 26
                    //Convert it to the space character
                    if(text_int - key_int < 0 && ((text_int - key_int + 27) % 27) == 26){
                        plain_text[i] = 32;
                    }
                    else if(text_int - key_int >= 0 && ((text_int - key_int) % 27) == 26){
                        plain_text[i] = 32;
                    }
                    else if(text_int - key_int < 0 && ((text_int - key_int + 27) % 27) != 26){
                        plain_text[i] = (((text_int - key_int) + 27) % 27) + 65;
                    }
                    else{
                        plain_text[i] = ((text_int - key_int) % 27) + 65;
                    }
                }

                //Add "@@" onto the end of the full buffer
                strcat(plain_text, "@@");

                //Send the plain_text back to the client's stdout with the @@ intact so that
                //the client knows when the buffer has ended
                charsWritten = send(connectionSocket, plain_text, strlen(plain_text), 0);
                if(charsWritten < 0){
                    fprintf(stderr, "ERROR writing to socket\n");
                }
                if(charsWritten < strlen(plain_text)){
                    fprintf(stderr, "SERVER: WARNING: Not all data written to socket!\n");
                }
            }
            exit(0);
            break;
        }
            //Close the connection socket for this client and terminates child processes
            wait(NULL);
            close(connectionSocket);
        }
    //Close the listening socket
    close(listenSocket);
    return 0;
}
