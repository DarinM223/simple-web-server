#include "parse.h"

/*
 * Test for parse.h
 */

int main() {
        /*
         * Test parseRequestMessage
         */
        char str[1000] = "GET /finalscore.jpg blahblahblah\nblahblahblah blahblahblah blahblahblah";
        char *newstr;

        long size;
        newstr = parseRequestMessage(str, &size);
        char *content_type = getFileTypeFromPath("finalscore.jpg");
        if (!content_type) {
                printf("content type is null for some reason");
        }
        //char *res = appendHeaderToResponse(newstr, content_type);

        printf("Response: %s %d\n", newstr, isImage(content_type)); 

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


        //char *response = appendHeaderToResponse("Hello world!", "text/html", strlen("Hello world!"));
        //printf("%s\n", response);

        return 0;
}
