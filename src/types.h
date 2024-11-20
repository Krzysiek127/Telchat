#ifndef H_TYPES
#define H_TYPES

#include "include.h"

#define MT_MAX_USERNAME 24
#define MT_MAX_BODY     80
#define TOAST_LENGTH    (MT_MAX_USERNAME + 80)

#define PROTO_VERSION   75  // Why? because i like

typedef struct {
    uint8_t uc_proto;
    time_t  tm_timestamp;

    wchar_t wsr_username[MT_MAX_USERNAME],
            wsr_address[MT_MAX_USERNAME],       // all 0s - wildcard '*'
            wsr_body[MT_MAX_BODY];
    
    uint64_t ul_chunk;
    uint8_t uc_type;
    uint32_t u_arg;
} message_t;
#define MD_MOTD_LENGTH  24

typedef struct {
    char hostname[256];
    uint32_t ipaddr;

    wchar_t motd_body[MD_MOTD_LENGTH];
} motd_t;


#define INFO_TEXT           0
#define INFO_DATA_BEGIN     1
#define INFO_DATA           2
#define INFO_DATA_END       3

#define INFO_DATA_ACK       4
#define INFO_DATA_ERROR     5

#define INFO_CONNECT    6
#define INFO_DXCONNECT  7

#define INFO_TXPING     8
#define INFO_RXPING     9

#define INFO_BROADCAST  10
#define INFO_PTY        11

#define FOREGROUND_DEFAULT  (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

#endif