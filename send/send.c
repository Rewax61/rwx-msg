#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void send_msg(FILE * __stream, char * _name_, char * _dest_, char * _msg_);
void get_name(char * _name_);
void get_msg(char * _msg_, char * _dest_);
void get_dest(char * _dest_);

int main(int argc, char **argv) {
    /* Nom de l'utilisateur */
    char name[50];
    if (argc > 1) {
        strcpy(name, argv[1]);
    } else {
        get_name(name);
    }
    /* Destinataire */
    char dest[50];
    if (argc > 2) {
        strcpy(dest, argv[2]);
    } else {
        get_dest(dest);
    }

    while (1) {
        FILE * chat = popen("ssh rewax61_chat@ssh-rewax61.alwaysdata.net /bin/bash > remoteoutput.txt", "w");
        /* Obtention du message par l'utilisateur */
        char msg[500];
        get_msg(msg, dest);

        if (! strcmp(msg, ":exit")) {
            //fprintf(chat, "mv ~/messages/%s.txt ~/messages/history/%s\\ $(date +%s);", name, name, "%s");
            fclose(chat);
            break;
        } /* Commande pour arrêter le programme -> :exit */

        /* Envoi du message au destinataire */
        send_msg(chat, name, dest, msg);
        fclose(chat);
    }
    

    return 0;
}


void get_name(char * _name_) {
    printf("Nom: ");
    fgets(_name_, 50, stdin);
    _name_[strlen(_name_)-1] = '\0';
}


void get_msg(char * _msg_, char * _dest_) {
    printf("%s > ", _dest_);
    fgets(_msg_, 500, stdin);
    _msg_[strlen(_msg_)-1] = '\0';
}


void get_dest(char * _dest_) {
    printf("Destinataire: ");
    fgets(_dest_, 50, stdin);
    _dest_[strlen(_dest_)-1] = '\0';
}


void send_msg(FILE * __stream, char * _name_, char * _dest_, char * _msg_) {
    fprintf(__stream, "echo \"%s: %s\" >> ~/messages/%s.txt;", _name_, _msg_, _dest_);
}
