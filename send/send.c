#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libssh/libssh.h>

void command(char * _cmd_, ssh_session *_session_, ssh_channel *_channel_, char * _result_);
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

    /* libssh */
    ssh_session session;
    int rc;

    // Create a new SSH session
    session = ssh_new();
    if (session == NULL) {
        return 1;
    }

    /* Set options for the SSH session (e.g., hostname, username) */
    ssh_options_set(session, SSH_OPTIONS_HOST, "ssh-rewax61.alwaysdata.net");
    ssh_options_set(session, SSH_OPTIONS_USER, "rewax61_chat");

    /* Connect to the remote server */
    rc = ssh_connect(session);
    if (rc != SSH_OK) {        
        ssh_free(session);
        return 2;
    }

    /* Authenticate with the remote server */
    rc = ssh_userauth_publickey_auto(session, NULL, NULL);
    if (rc != SSH_AUTH_SUCCESS) {        
        ssh_disconnect(session);
        ssh_free(session);
        return 3;
    }

    /* Création de la variable pour le channel */
    ssh_channel channel;

    char result[100];
    char cmd[300];

    while (1) {
        
        /* Obtention du message par l'utilisateur */
        char msg[500];
        get_msg(msg, dest);

        if (! strcmp(msg, ":exit")) { /* Commande pour arrêter le programme -> :exit */
            //printf("%s", cmd);
            break;
        }

        /* Envoi du message au destinataire */
        strcpy(cmd, "echo \""); strcat(cmd, name); strcat(cmd, ": "); strcat(cmd, msg); strcat(cmd, "\" >> ~/messages/"); strcat(cmd, dest); strcat(cmd, ".txt;");
        command(cmd, &session, &channel, result);
    }

    /* Disconnect and free the SSH session */
    ssh_disconnect(session);
    ssh_free(session);

    return 0;
}

void command(char * _cmd_, ssh_session *_session_, ssh_channel *_channel_, char * _result_) {
    int rc;
    //ssh_channel _channel_;
    *_channel_ = ssh_channel_new(*_session_);
    if (*_channel_ == NULL) {        
        ssh_disconnect(*_session_);
        ssh_free(*_session_);
        exit(4);
    }
    rc = ssh_channel_open_session(*_channel_);
    if (rc != SSH_OK) {        
        ssh_channel_free(*_channel_);
        ssh_disconnect(*_session_);
        ssh_free(*_session_);
        exit(5);
    }


    /* Execute a command on the remote server */
    rc = ssh_channel_request_exec(*_channel_, _cmd_);
    if (rc != SSH_OK) {
        exit(6);
    }
    
    /* Output */
    char res_cmd[256];
    int nbytes;
    while ((nbytes = ssh_channel_read(*_channel_, res_cmd, sizeof(res_cmd), 0)) > 0) {
        printf("Output: %.*s", nbytes, res_cmd);
    }
    if (nbytes < 0) {
        exit(7);
    }

    /* Close the SSH channel */
    ssh_channel_send_eof(*_channel_);
    ssh_channel_close(*_channel_);
    ssh_channel_free(*_channel_);

    strcpy(_result_, res_cmd);
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
