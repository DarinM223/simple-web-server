#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXT_STR_SIZE 10

/* appends a character to the string, and resizes if necessary 
 * str_size is the capacity 
 * returns 0 if it works, -1 if it doesn't */
int append(int ch, char** str, int *str_size) /*tested and works*/
{       int size = strlen(*str); /*get the size of the string*/
      if ((size+1) < *str_size) { /*if the size (including the zero byte) is less than the capacity*/
              (*str)[size+1] = (*str)[size]; /*set the space after the previous zero byte to a zero byte*/
              (*str)[size] = ch; /*set the previous zero byte space to the character*/
      } else {
              /*resize the string*/
              *str_size *= 2; /*double the capacity*/
              char *newStr = (char*)realloc(*str,*str_size * sizeof(char));
              if (newStr == NULL) {return -1;}
              else {
                      *str = newStr;
              }
              
              (*str)[size+1] = (*str)[size]; /*set the space after the previous zero byte to a zero byte*/
              (*str)[size] = ch; /*set the previous zero byte space to the character*/
      }
      return 0;
}

/*
 * Returns 1 if contenttype is an image (like "image/gif") and 0 if
 * contenttype is not an image (like "text/html")
 */
int isImage(char *contenttype)
{
        char *firstword = strtok(contenttype, "/");
        return (firstword != NULL ? (strcmp(firstword, "image") == 0) : 0);
}

/*
 * Returns the string with an html header appended to it
 */
char *appendHeaderToResponse(char* str, char* filetype, long *str_size)
{
        char ok_response[100] = "HTTP/1.1 200 OK\nContent-Type: ";
        //length of returned string
        long returnLen = *str_size + strlen(filetype) + strlen(ok_response) + 2 + 1;
        char *returnStr = (char*)malloc(returnLen*sizeof(char));
        sprintf(returnStr, "%s%s\n\n", ok_response, filetype);

        if (!isImage(filetype)) {
                strcat(returnStr, str);
        } else {
                memcpy(returnStr, str, *str_size);
        }
        //change total size to be that of str + the header
        *str_size = returnLen;
        return returnStr;
}


/*
 * Code attempts to find the extension of the file being requested
 * like "index.html" or "hello.jpg" or "user/pics/sexy.gif"
 * 
 * Returns NULL if no content type matches and the content type if it does match
 */
char *getFileTypeFromPath(char *path) 
{
        char *filetype;
        char html_file_type[20] = "text/html";
        char gif_file_type[20] = "image/gif";
        char jpg_file_type[20] = "image/jpeg";
        int i;
        if (strlen(path) > 1) {
               int dotindex = -1;
               //go backward until a '.' is found stop if a '/' is found
               for (i = (strlen(path) - 1); i >= 0; i--) {
                     if (path[i] == '.') {
                             dotindex = i;
                             break;
                     } else if (path[i] == '/') {
                             break;
                     }
               }
               //if dot wasn't set do same thing as if the length of the
               //path was less than 2
               if (dotindex == -1) {
                     return NULL;
               } else {
                       //go and append each of the characters to the string
                       char *extension = (char*) malloc(EXT_STR_SIZE * sizeof(char));
                       strcpy(extension, "");
                       int size = EXT_STR_SIZE;

                       for (i = dotindex; i < strlen(path); i++) {
                               append(path[i], &extension, &size);
                       }

                       printf("Extension: %s\n", extension);

                       if (strcmp(extension, ".gif") == 0) {
                               filetype = (char*)malloc((strlen(gif_file_type)+1)*sizeof(char));
                               strcpy(filetype, gif_file_type);
                       } else if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
                               filetype = (char*)malloc((strlen(jpg_file_type)+1)*sizeof(char));
                               strcpy(filetype, jpg_file_type);
                       } else if (strcmp(extension, ".html") == 0) {
                               filetype = (char*)malloc((strlen(html_file_type)+1)*sizeof(char));
                               strcpy(filetype, html_file_type);
                       } else {
                               return NULL;
                       }
               }
        } else { 
                return NULL;
        }
        return filetype;
}

/*
 * This function parses the request message, finds the html file in the directory, and returns the file as text
 */
char* parseRequestMessage(char* request, long *size)
{
        FILE *fp = NULL;
        char *contents = NULL;
        long file_size = 0;
        char *filetype;

        char *firstword = strtok(request, " ");
        firstword = NULL;

        char *secondword = strtok(NULL, " ");
        printf("second word: %s\n", secondword);

        char *path = (char*)malloc((strlen(secondword)+1) * (sizeof(char)));
        if (strlen(secondword) > 1) {
                strcpy(path, (secondword[0] == '/' ? secondword+1 : secondword));
        } else {
                return NULL;
        }

        printf("Path is : %s\n", path);
        filetype = getFileTypeFromPath(path); //filetype can be NULL!!

        //analyze path

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

        if (filetype) {
                //file_size will increase by the header amount
                char *completeReq = appendHeaderToResponse(contents, filetype, &file_size);
                //sets the size to the total new size
                *size = file_size;
                return completeReq;
        } else {
                return NULL;
        }
}
