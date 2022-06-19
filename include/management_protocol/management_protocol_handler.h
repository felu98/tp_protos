#ifndef MANAGEMENT_PROTOCOL_HANDLER_H
#define MANAGEMENT_PROTOCOL_HANDLER_H

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "args_handler.h"
#include "../parsers/admin_get.h"
#include "../parsers/admin_configstatus.h"

#define MAX_FIELD_SIZE 256
#define SERVER_ERROR 0xFF

#define GET_MSG_SIZE 3
#define PUT_MSG_SIZE 4
#define EDIT_MSG_SIZE 6
#define CONFIGSTATUS_MSG_SIZE 4
#define CONFIGBUFFSIZE_MGS_SIZE 4

char* get_msg[] = {"Success", "Invalid action", "Invalid option"};
char* put_msg[] = {"Success", "Invalid action", "Invalid username length", "Invalid password length"};
char* edit_msg[] = {"Success", "Invalid action", "Invalid field", "Invalid username length", "Invalid attribute", "Invalid value length"};
char* configstatus_msg[] = {"Success", "Invalid action", "Invalid field", "Invalid status"};
char* configbuffsize_msg[]= {"Success", "Invalid action", "Invalid buffer size length", "Invalid buffer size"};

// Chequear desp cuando mergeemos branches
// Agregar STATUS_ERROR_IN_PASS
enum connection_status {
    CONN_STATUS_OK = 0x00,
    CONN_STATUS_ERROR_IN_VERSION = 0x01,
    CONN_STATUS_ERROR_IN_PASSLEN = 0x02,
    CONN_STATUS_ERROR_IN_PASS = 0x03,
};

enum get_status {
    GET_STATUS_OK = 0x00,
    GET_STATUS_ERROR_INVALID_ACTION = 0x01,
    GET_STATUS_ERROR_INVALID_OPTION = 0x02,
    GET_STATUS_SERVER_ERROR = 0x03
} get_status;

enum list_options {
    USERS,
    PASSWORDS,
    BUFFERSIZE,
    AUTH_STATUS,
    SPOOFING_STATUS,
} list_options;

enum get_options {
    SENT_BYTES,
    RECEIVED_BYTES,
    HISTORIC_CONNECTIONS,
    CONCURRENT_CONNECTIONS,
} get_options;

void login(int fd, struct manage_args* args);
void executeCommands(int fd, struct manage_args* args);

#endif