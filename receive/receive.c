#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<time.h>

#define sleep(nb_seconds) clock_t goal = (((nb_seconds) * CLOCKS_PER_SEC) + clock()); while (goal > clock()) {;}

void get_name(char * _name_);
void command(char * _cmd_, char * _result_);

int main(int argc, char **argv) {
    char name[50];
    if (argc != 1) {
        strcpy(name, argv[1]);
    } else {
        get_name(name);
    }


    /* Création du fichier de réception */
    FILE * chat = popen("ssh rewax61_chat@ssh-rewax61.alwaysdata.net /bin/bash > remoteoutput.txt", "w");
    fprintf(chat, "touch ~/messages/%s.txt;", name);
    fclose(chat);

    char hash[300]; hash[0] = '\0'; /* Définition de hash */
    char old_hash[300]; old_hash[0] = '\0'; /* Définition de old_hash */
    for (;;) {
        char cmd[300];
        strcpy(cmd, "shasum -a 256 ~/messages/"); strcat(cmd, name); strcat(cmd, ".txt;");
        command(cmd, hash);
        if (strcmp(hash, old_hash)) {
            strcpy(cmd, "tail -n 1 ~/messages/"); strcat(cmd, name); strcat(cmd, ".txt;");
            char message[300];
            command(cmd, message);
            printf("%s\n", message);
        }
        strcpy(old_hash, hash);
        sleep(1);
    }

    return 0;
}


void command(char * _cmd_, char * _result_) {
    FILE * chat = popen("ssh rewax61_chat@ssh-rewax61.alwaysdata.net /bin/bash > remoteoutput.txt", "w");
    fprintf(chat, _cmd_);
    fclose(chat);
    FILE * output = fopen("remoteoutput.txt", "r");
    char res_cmd[300];
    fgets(res_cmd, 300, output);
    res_cmd[strlen(res_cmd)-1] = '\0';
    strcpy(_result_, res_cmd);
}


void get_name(char * _name_) {
    printf("Nom: ");
    fgets(_name_, 50, stdin);
    _name_[strlen(_name_)-1] = '\0';
}