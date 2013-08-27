#include <stdio.h>
#include <stdlib.h>
#include "rotaryencoder.h"
#include <curl/curl.h>
#include <string.h>
int main()
{
CURL *curl;
CURLcode res;
printf("Hello!\n");
wiringPiSetup () ;
/*using pins 23/24*/
struct encoder *encoder = setupencoder(4,5);
long value;
char buffer[250];
char buffer2[250];
while (1)
 {
 updateEncoders();
 long l = encoder->value;
 if(l!=value)
 {
     printf("value: %d\n", (void *)l);
     curl=curl_easy_init();
     sprintf(buffer, "http://127.0.0.1/jsonrpc?request={\"jsonrpc\":\"2.0\",\"method\":\"Application.SetVolume\",\"params\":{\"volume\":%d},\"id\":1}", l);
     printf ("testing %s", buffer);
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

