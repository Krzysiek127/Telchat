#ifndef H_MESSAGE
#define H_MESSAGE

#include "types.h"

message_t *msg_makeraw(void);
message_t *msg_addbody(message_t *in, wchar_t *body, size_t length);
message_t *msg_setChunk(message_t *in, uint64_t chnk);
message_t *msg_setType(message_t *in, uint8_t t);
message_t *msg_setArgument(message_t *in, uint32_t arg);
message_t *msg_addRecp(message_t *in, wchar_t *usr);

void msg_sendfile(SOCKET msock, LPCWSTR filename);

int mesprintf(message_t *msg);

#endif