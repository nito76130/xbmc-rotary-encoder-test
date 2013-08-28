#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "rotaryencoder.h"

#include <curl/curl.h>
#include <string.h>


int numberofencoders = 0;

void updateEncoders()
{
    struct encoder *encoder = encoders;
    for (; encoder < encoders + numberofencoders; encoder++)
    {
        int MSB = digitalRead(encoder->pin_a);
        int LSB = digitalRead(encoder->pin_b);

        int encoded = (MSB << 1) | LSB;
        int sum = (encoder->lastEncoded << 2) | encoded;
        long value = (encoder->value);

        if((sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) && (value < 100))
        {
            encoder->value++;
        }
        if((sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)  && (value > 0))
        {
            encoder->value--;
        }
        encoder->lastEncoded = encoded;
    }
}

struct encoder *setupencoder(int pin_a, int pin_b)
{
    if (numberofencoders > max_encoders)
    {
        printf("Maximum number of encodered exceded: %i\n", max_encoders);
        return NULL;
    }

    struct encoder *newencoder = encoders + numberofencoders++;
    newencoder->pin_a = pin_a;
    newencoder->pin_b = pin_b;
    newencoder->value = 0;
    newencoder->lastEncoded = 0;

    pinMode(pin_a, INPUT);
    pinMode(pin_b, INPUT);
    pullUpDnControl(pin_a, PUD_UP);
    pullUpDnControl(pin_b, PUD_UP);
    wiringPiISR(pin_a,INT_EDGE_BOTH, updateEncoders);
    wiringPiISR(pin_b,INT_EDGE_BOTH, updateEncoders);

    return newencoder;
}

int main()
{
CURL *curl;
CURLcode res;
wiringPiSetup () ;
/*using pins 23/24*/
struct encoder *encoder = setupencoder(4,5);
long value;
char buffer[250];
while (1)
 {
 updateEncoders();
 long l = encoder->value;
 if(l!=value)
 {
     printf("value: %d\n", (void *)l);
     curl=curl_easy_init();
     sprintf(buffer, "http://127.0.0.1/jsonrpc?request={\"jsonrpc\":\"2.0\",\"method\":\"Application.SetVolume\",\"params\":{\"volume\":%d},\"id\":1}", l);
     curl_easy_setopt(curl, CURLOPT_URL, buffer);
    /* Perform the request, res will get the return code */
     res = curl_easy_perform(curl);
     /* Check for errors */
     if(res != CURLE_OK)
     {
         fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
     }
     /* always cleanup */
     curl_easy_cleanup(curl);
     value = l;
  }
 }
return(0);
}

