#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAXLEN 1000

int main(int argc, char *argv[]) {
        if (argc !=2) {
                puts("Usage: gclient <server-fifo-name>");
                exit(1);
        }

        // argv[1] is the server fifo name

    char clientfifo[MAXLEN];
        sprintf(clientfifo, "/tmp/%s-%d", getenv("USER"), getpid());
        mkfifo(clientfifo, 0600);
        chmod(clientfifo, 0622);

        FILE *fp = fopen(argv[1], "w");

        fprintf(fp, "%s\n", clientfifo);
        fclose(fp);

        //open clientfifo for reading and read the quote & print in the screen - improve your life! :-)
        FILE *clientfp = fopen(clientfifo, "r");
        //read the new server-fifo, then open it for writing! as serverfp
        char serverfifo[MAXLEN];
        //fgets(serverfifo, clientfp); -- fscanf() is better! No need to worry about \n
        fscanf(clientfp, "%s", serverfifo);
        char line[MAXLEN];
        fgets(line, MAXLEN, clientfp); //get rid of \n

        FILE *serverfp = fopen(serverfifo, "w");
        
        char word[] = "(Guess)";
        char secWord[] = "The";
        while (1) {
                char letterInput;
                fgets(line, MAXLEN, clientfp);
                if (strncmp(line, word, strlen(word)) == 0) {
                        char *cptr = strchr(line, "\n");
                        if (cptr) {
                                *cptr = '\0';
                        }

                        puts(line);
                        char buffer[10];
                        scanf("%s", buffer);

                        fprintf(serverfp, "%c\n", buffer[0]);
                        fflush(serverfp);
                }
                else if (strncmp(line, secWord, strlen(secWord)) == 0) {
                        printf("%s\n", line);
                        break;
                }
                else {
                        printf(line);
                }

        }

        fclose(clientfp);
        unlink(clientfifo);
        exit(0);
}
