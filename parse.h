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
 * Returns 1 if the first word of a content type (image in image/gif or text in text/html)
 * is equal to the filetype parameter
 */
int isType(char *contenttype, char *filetype)
{
        char *firstword = strtok(contenttype, "/");
        return (firstword != NULL ? (strcmp(firstword, filetype) == 0) : 0);
}

/*
 * Returns the string with an html header appended to it
 */
char *appendHeaderToResponse(char* content, char* contenttype, long *content_size)
{
        char ok_response[30] = "HTTP/1.1 200 OK\nContent-Type: ";
        //length of returned string
        long returnLen = *content_size + strlen(contenttype) + strlen(ok_response) + 2 + 1;
        char *returnStr = (char*)malloc(returnLen*sizeof(char));
        sprintf(returnStr, "%s%s\n\n", ok_response, contenttype);

        if (isType(contenttype, "text")) { //if text or html file, concat string
                strcat(returnStr, content);
        } else if (isType(contenttype, "image")) { //if image, memcpy the image data
                memcpy(returnStr+strlen(returnStr), content, *content_size);
        } else if (isType(contenttype, "video")) { //if video, memcpy the video data
                memcpy(returnStr+strlen(returnStr), content, *content_size);
        } else { //otherwise, return NULL
                *content_size = 0;
                return NULL;
        }
        //change total size to be that of content + the header
        *content_size = returnLen;
        //printf("new content size: %d\n", *content_size);
        return returnStr;
}


/*
 * Code attempts to find the extension of the file being requested
 * like "index.html" or "hello.jpg" or "user/pics/sexy.gif"
 * 
 * Returns NULL if no content type matches and the content type if it does match
 */
char *getContentTypeFromPath(char *path) 
{
        char *contenttype;
        char html_file_type[20] = "text/html";
        char gif_file_type[20] = "image/gif";
        char jpg_file_type[20] = "image/jpeg";
        char flv_file_type[20] = "video/x-flv";
        char avi_file_type[20] = "video/avi";
        char mkv_file_type[20] = "video/x-matroska";
        char mp4_file_type[20] = "video/mp4";
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
                     //return NULL;
                     contenttype = NULL;
               } else {
                       //go and append each of the characters to the string
                       char *extension = (char*) malloc(EXT_STR_SIZE * sizeof(char));
                       strcpy(extension, "");
                       int size = EXT_STR_SIZE;

                       for (i = dotindex; i < strlen(path); i++) {
                               append(path[i], &extension, &size);
                       }

                       //printf("Extension: %s\n", extension);

                       if (strcmp(extension, ".gif") == 0) {
                               contenttype = (char*)malloc((strlen(gif_file_type)+1)*sizeof(char));
                               strcpy(contenttype, gif_file_type);
                       } else if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
                               contenttype = (char*)malloc((strlen(jpg_file_type)+1)*sizeof(char));
                               strcpy(contenttype, jpg_file_type);
                       } else if (strcmp(extension, ".html") == 0) {
                               contenttype = (char*)malloc((strlen(html_file_type)+1)*sizeof(char));
                               strcpy(contenttype, html_file_type);
                       } else if (strcmp(extension, ".flv") == 0) {
                               contenttype = (char*)malloc((strlen(flv_file_type)+1)*sizeof(char));
                               strcpy(contenttype, flv_file_type);
                       } else if (strcmp(extension, ".avi") == 0) {
                               contenttype = (char*)malloc((strlen(avi_file_type)+1)*sizeof(char));
                               strcpy(contenttype, avi_file_type);
                       } else if (strcmp(extension, ".mkv") == 0) {
                               contenttype = (char*)malloc((strlen(mkv_file_type)+1)*sizeof(char));
                               strcpy(contenttype, mkv_file_type);
                       } else if (strcmp(extension, ".mp4") == 0) {
                               contenttype = (char*)malloc((strlen(mp4_file_type)+1)*sizeof(char));
                               strcpy(contenttype, mp4_file_type);
                       } else {
                               contenttype = NULL;
                               //return NULL;
                       }
                       free(extension);
                       //printf("Content type: %s\n", contenttype);
               }
        } else { 
                //return NULL;
                contenttype = NULL;
        }
        return contenttype;
}

/*
 * This function parses the request message, finds the html file in the directory, and returns the file as text
 */
char* parseRequestMessage(char* request, long *size)
{
        FILE *fp = NULL;
        char *contents = NULL;
        long file_size = 0;
        char *contenttype;

        char *firstword = strtok(request, " ");
        firstword = NULL;

        char *secondword = strtok(NULL, " ");

        //if there is no second word, return NULL
        if (secondword == NULL) {
                return NULL;
        }
        //printf("second word: %s\n", secondword);

        char *path = (char*)malloc((strlen(secondword)+1) * (sizeof(char)));

        //remove the first '/' if there is one
        if (strlen(secondword) > 1) {
                strcpy(path, (secondword[0] == '/' ? secondword+1 : secondword));
        } else {
                return NULL;
        }

        //if even after stripping off the first '/' there is another '/' then
        //they are trying to access root
        if (strlen(path) > 1 && path[0] == '/') {
                printf("Attempted to access root!\n");
                return NULL;
        }
        //check for .. or ~/
        int numdots = 0;
        int i;
        for (i = 0; i < strlen(path); i++) {
               if (path[i] == '.') {
                       numdots++;
               } else if (path[i] == '~') {
                       printf("Attempted to access home directory!\n");
                       return NULL;
               } else { //reset numdots if character is not dot
                       numdots = 0;
               }
               if (numdots >= 2) {
                       printf("Attempted to go up a directory!\n");
                       return NULL;
               }
        }

        //printf("Path is : %s\n", path);
        contenttype = getContentTypeFromPath(path); //contenttype can be NULL!!

        //analyze path

        //open the file from the second word (taking of the first '/')
        fp = fopen(path, "rb");
        if (fp == NULL) 
                return NULL;
                //exit(EXIT_FAILURE);


        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
        rewind(fp);

        contents = (char*) malloc(file_size * (sizeof(char)));
        fread(contents, sizeof(char), file_size, fp);
        fclose(fp);

        if (contenttype) {
                //file_size will increase by the header amount
                char *completeReq = appendHeaderToResponse(contents, contenttype, &file_size);
                free(path);
                free(contenttype);
                free(contents);
                path = NULL;
                contenttype = NULL;
                contents = NULL;

                //sets the size to the total new size
                *size = file_size;
                return completeReq;
        } else {
                free(path);
                free(contenttype);
                free(contents);
                path = NULL;
                contenttype = NULL;
                contents = NULL;

                return NULL;
        }
}
