#include "parse.h"

/*
 * Test for parse.h
 */

int main() {
        /*
         * Test parseRequestMessage
         */
        char str[1000] = "GET /index.html blahblahblah\nblahblahblah blahblahblah blahblahblah";
        char *newstr;

        newstr = parseRequestMessage(str);

        printf("Response: %s\n", newstr); 

        /*
         * Test getFileTypeFromPath 
         */
        char *filetype = getFileTypeFromPath("user/pics/sexy.gif");
        char *filetype2 = getFileTypeFromPath("a");
        char *filetype3 = getFileTypeFromPath("hello.blaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaah");
        char *filetype4 = getFileTypeFromPath("hello/pics/sexy.gif/");
        printf("Filetype 1: %s\n", filetype);
        printf("Filetype 2: %s\n", filetype2);
        printf("Filetype 3: %s\n", filetype3);
        printf("Filetype 4: %s\n", filetype4);


        char *response = appendHeaderToResponse("Hello world!", "text/html");
        printf("%s\n", response);

        return 0;
}
