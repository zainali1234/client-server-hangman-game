#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAXWORDS 100000
#define MAXLEN 1000

int missed = 0;
char *words[MAXWORDS];
int numWords=0;

int charString(char c, const char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == c) {
            return 1;  // character found in string
        }
    }
    return 0;  // character not found in string
}

int main() {
    char line[MAXLEN];


    FILE *fp = fopen("dictionary.txt", "r");
    if (!fp) {
            puts("dictionary.txt cannot be opened for reading.");
            exit(1);
    }

    /*
    while (numWords < MAXWORDS && fscanf(fp, "%s", words[numWords]) == 1) {
            numWords++;
    }
    */

    int i=0;

    //read one line at a time, allocate memory, then copy the line into array
    while (fgets(line, MAXLEN, fp)) {
            char *cptr = strchr(line, '\n');
            if (cptr)
                    *cptr = '\0';
            words[i] = (char *) malloc (strlen(line)+1);
            strcpy(words[i], line);
            i++;
    }
    numWords = i;
    printf("%d words were read.\n", numWords);

    srand(getpid() + time(NULL) + getuid());
    // puts(quotes[rand() % numQuotes]);

    char randomWord[100]; // initialize with null character
    strcpy(randomWord, words[rand() % numWords]); // copy contents of words array

    char hiddenWord[100];

    for (int i = 0; i < strlen(randomWord); i++) {
            hiddenWord[i] = '*';
    }
    hiddenWord[strlen(randomWord)] = '\0';

    printf("%s\n", randomWord);

    char filename[MAXLEN];
    sprintf(filename, "/tmp/%s-%d", getenv("USER"), getpid());
    mkfifo(filename, 0600);
    chmod(filename, 0622);
    printf("Send your requests to %s\n", filename);

    //char otherWord[100] = "*****";
    while (1) {
        FILE *fp = fopen(filename, "r");
        if (!fp) {
                printf("FIFO %s cannot be opened for reading.\n", filename);
                exit(2);
        }
        printf("Opened %s to read...\n", filename);

        //wait for clients' requests
        while (fgets(line, MAXLEN, fp)) {
            char *cptr = strchr(line, '\n');
            if (cptr)
                    *cptr = '\0';

            //create a child to work with this client
            if (fork() == 0) {
                    FILE *clientfp = fopen(line, "w");
                    //create and send new server fifo to the client
                    //for private one-on-one communcations
                    char serverfifo[MAXLEN];
                    sprintf(serverfifo, "/tmp/%s-%d", getenv("USER"), getpid());
                    mkfifo(serverfifo, 0600);
                    chmod(serverfifo, 0622);

                    fprintf(clientfp, "%s\n", serverfifo);
                    fflush(clientfp);

                    FILE *serverfp = fopen(serverfifo, "r");
                    char requestChar = '#';

                    do {
                            int found = 0;

                            for (i = 0; i < strlen(randomWord); i++) {
                                    if (requestChar == randomWord[i]) {
                                            if (hiddenWord[i] == '*') {
                                                    hiddenWord[i] = randomWord[i];
                                                    found = 1;
                                            }
                                            else if (hiddenWord[i] == requestChar) {
                                                    found = 2;
                                            }
                                    }
                            }

                            if (found == 2) {
                                    fprintf(clientfp, "%c is already in the word\n", requestChar);
                            }

                            if (!found && requestChar != '#') {
                                    fprintf(clientfp, "%c is not in the word\n", requestChar);
                                    missed++;
                            }

                            if (!charString('*', hiddenWord)) {
                                    fprintf(clientfp, "The word is %s. You missed %d times", hiddenWord, missed);
                                    unlink(serverfifo);
                                    fclose(fp);
                                    exit(0);
                            }

                            fprintf(clientfp, "(Guess) Enter a letter in word %s>\n", hiddenWord);
                            fflush(clientfp);
                            //printf("%s, %s %d: %s", serverfifo, line, requestNum, hiddenWord);
                    } while (fscanf(serverfp, "%s", &requestChar));
                    unlink(serverfifo);
                    exit(0);
                }
            }
            fclose(fp);
    }
}
        
