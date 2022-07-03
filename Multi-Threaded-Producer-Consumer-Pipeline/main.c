//Daniel Valadez, 5-15-22, CS344
//This is the main driver for this programing assignment.
//The purporse of this program is to use 4 threads to process
//input in a way that we can alter what is outputted through these
//four different threads having different responsibilities.

#include "program4.h"

int main(){
    //Code below will be very similar to the sample program given to us as
    //an example. Here it is being altered to create four threads instead of three.
    srand(time(0));

    //Now I can decalre the names of my threads...
    pthread_t input, lineSeparator, plusSign, output;

    //Create the threads...
    pthread_create(&input, NULL, get_input, NULL); //get_input is defined in another file.
    pthread_create(&lineSeparator, NULL, separator, NULL);
    pthread_create(&plusSign, NULL, replace, NULL);
    pthread_create(&output, NULL, writeOutput, NULL);

    //Now wait for the threads to terminate...
    pthread_join(input, NULL);
    pthread_join(lineSeparator, NULL);
    pthread_join(plusSign, NULL);
    pthread_join(output, NULL);
    return EXIT_SUCCESS;
}
