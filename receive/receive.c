#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <libssh/libssh.h>

#define sleep(nb_seconds) clock_t goal = (((nb_seconds) * CLOCKS_PER_SEC) + clock()); while (goal > clock()) {;}


void get_name(char * _name_);
void command(char * _cmd_, ssh_session *_session_, ssh_channel *_channel_, char * _result_);

int main(int argc, char **argv) {
    char name[50];
    if (argc != 1) {
        strcpy(name, argv[1]);
    } else {
        get_name(name);
    }

    ssh_session session;
    int rc;

    // Create a new SSH session
    session = ssh_new();
    if (session == NULL) {
        return 1;
    }

    // Set options for the SSH session (e.g., hostname, username)
    ssh_options_set(session, SSH_OPTIONS_HOST, "ssh-rewax61.alwaysdata.net");
    ssh_options_set(session, SSH_OPTIONS_USER, "rewax61_chat");

    // Connect to the remote server
    rc = ssh_connect(session);
    if (rc != SSH_OK) {        
        ssh_free(session);
        return 2;
    }

    // Authenticate with the remote server
    rc = ssh_userauth_publickey_auto(session, NULL, NULL);
    if (rc != SSH_AUTH_SUCCESS) {        
        ssh_disconnect(session);
        ssh_free(session);
        return 3;
    }

    // Création de la variable pour le channel
    ssh_channel channel;


    /* Création du fichier de réception */
    char init_cmd[100];
    char result[100];
    strcpy(init_cmd, "touch ~/messages/"); strcat(init_cmd, name); strcat(init_cmd, ".txt");
    command(init_cmd, &session, &channel, result);
    
    sleep(1);
    char content_file[300]; content_file[0] = '\0';
    char cmd[300];
    while (1) {
        strcpy(content_file, "");
        content_file[0] = '\0';
        cmd[0] = '\0';
        strcpy(cmd, "cat ~/messages/"); strcat(cmd, name); strcat(cmd, ".txt;");
        command(cmd, &session, &channel, content_file);
        if (strcmp(content_file, "") != 0) {
            printf("%s", content_file);
            strcpy(cmd, "echo -n "" > ~/messages/"); strcat(cmd, name); strcat(cmd, ".txt;");
            command(cmd, &session, &channel, result);
        }
        sleep(2);
    }

    printf("La discussion a été interrompue !\n");

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
        //printf("Output: %.*s", nbytes, res_cmd);
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