#include "parse.h"

/*
 * Test for parse.h
 */

int main() {
        char str[1000] = "GET /index.html blahblahblah\nblahblahblah blahblahblah blahblahblah";
        char *newstr;

        newstr = parseRequestMessage(str);
        printf("Response: %s", newstr); 
        return 0;
}
