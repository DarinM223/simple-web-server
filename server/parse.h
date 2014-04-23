#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * This function parses the request message, finds the html file in the directory, and returns the file as text
 */
char* parseRequestMessage(char* request) {
        FILE *fp = NULL;
        char *contents = NULL;
        long file_size = 0;

        char *firstword = strtok(request, " ");
        firstword = NULL;

        char *secondword = strtok(NULL, " ");
        printf("second word: %s\n", secondword);

        char *path = (char*)malloc((strlen(secondword)) * (sizeof(char)));
        if (strlen(secondword) > 1) {
                strcpy(path, (secondword[0] == '/' ? secondword+1 : secondword));
        } else {
                return NULL;
        }

        printf("Path is : %s\n", path);

        //open the file from the second word (taking of the first '/')
        fp = fopen(path, "rb");

        if (fp == NULL) 
                exit(EXIT_FAILURE);

        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
        rewind(fp);

        contents = (char*) malloc(file_size * (sizeof(char)));
        fread(contents, sizeof(char), file_size, fp);
        fclose(fp);

        return contents;
}
