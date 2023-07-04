#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libssh/libssh.h>
#include<time.h>

#define sleep(nb_seconds) clock_t goal = (((nb_seconds) * CLOCKS_PER_SEC) + clock()); while (goal > clock()) {;}

int main() {
    
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

    // Open a channel for executing commands
    ssh_channel channel;
    channel = ssh_channel_new(session);
    if (channel == NULL) {        
        ssh_disconnect(session);
        ssh_free(session);
        return 4;
    }

    // Request a pseudo-terminal for the channel
    /* rc = ssh_channel_request_pty(channel);
    if (rc != SSH_OK) {
        printf("%d\n", SSH_OK);
        printf("%d\n", rc);
        
        ssh_channel_free(channel);
        ssh_disconnect(session);
        ssh_free(session);
        return 5;
    } */

    // Open the channel
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK) {        
        ssh_channel_free(channel);
        ssh_disconnect(session);
        ssh_free(session);
        return 6;
    }

    while (1) {
        ssh_channel channel;
        channel = ssh_channel_new(session);
        if (channel == NULL) {
            ssh_disconnect(session);
            ssh_free(session);
            return 7;
        }
        rc = ssh_channel_open_session(channel);
        if (rc != SSH_OK) {
            ssh_channel_free(channel);
            ssh_disconnect(session);
            ssh_free(session);
            return 8;
        }

        // Execute a command on the remote server
        rc = ssh_channel_request_exec(channel, "echo Hello World!");
        if (rc != SSH_OK) {
            break;
        }
        
        // Output
        char buffer[256];
        int nbytes;
        while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
            printf("Output: %.*s", nbytes, buffer);
        }
        if (nbytes < 0) {
            break;
        }

        // Close the SSH channel
        ssh_channel_send_eof(channel);
        ssh_channel_close(channel);
        ssh_channel_free(channel);

        sleep(1);
    }

    // Close the SSH channel
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    // Disconnect and free the SSH session
    ssh_disconnect(session);
    ssh_free(session);


    return 0;
}
