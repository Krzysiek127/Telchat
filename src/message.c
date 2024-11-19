#include "message.h"

extern wchar_t wsr_current_user[MT_MAX_USERNAME],
               wsr_toastbuffer[TOAST_LENGTH];


message_t *msg_makeraw(void) {
    message_t *tmp = calloc(1, sizeof(message_t));

    tmp->uc_proto = PROTO_VERSION;
    time(&tmp->tm_timestamp);
    wmemcpy(tmp->wsr_username, wsr_current_user, MT_MAX_USERNAME);

    //wcsncpy()
    return tmp;
}

message_t *msg_addRecp(message_t *in, wchar_t *usr) {
    wmemcpy(in->wsr_address, usr, MIN(wcslen(usr), MT_MAX_BODY));
    return in;
}

message_t *msg_addbody(message_t *in, wchar_t *body, size_t length) {
    wmemcpy(in->wsr_body, body, MIN(wcslen(body), MT_MAX_BODY));
    return in;
}

message_t *msg_setType(message_t *in, uint8_t t) {
    in->uc_type = t;
    return in;
}

message_t *msg_setArgument(message_t *in, uint32_t arg) {
    in->u_arg = arg;
    return in;
}

message_t *msg_setChunk(message_t *in, uint64_t chnk) {
    in->ul_chunk = chnk;
    return in;
}

//void msg_sendfile(message_t *in, SOCKET msock, FILE *file) {
//    fseek(file, 0L, SEEK_END);
//    uint32_t fsize = ftell(file);
//    rewind(file);
//
//    uint32_t chunkSize = fsize / (MT_MAX_BODY * sizeof(wchar_t));
//    uint32_t remainingChunk = fsize % (MT_MAX_BODY * sizeof(wchar_t));
//    
//
//    printf("DEBUG: CHUNK=%llu REM=%llu\n", chunkSize, remainingChunk);
//
//    message_t *begin = msg_makeraw();
//    begin = msg_setType(begin, INFO_DATA_BEGIN);
//    begin = msg_addbody(begin, )
//    send(msock, (char*)begin, sizeof(message_t), 0);
//    free(begin);
//
//    for (uint32_t ch = 0; ch < chunkSize; ch++) {
//        message_t *msg = msg_makeraw();
//        msg = msg_setflag(msg, INFO_DATA);
//        
//        uint64_t tmpck = (chunkSize << 32);
//        tmpck |= ch;
//        printf("UP=%08x LW=%08x\n", (tmpck >> 32), (tmpck & 0xFFFFFFFF));
//
//        msg = msg_setChunk(msg, tmpck);
//        fread(msg->wsr_body, 1, MT_MAX_BODY * sizeof(wchar_t), file);
//        send(msock, (char*)msg, sizeof(message_t), 0);
//    }
//    printf("END\n");
//
//}


void msg_sendfile(SOCKET msock, LPCWSTR filename) {
    HANDLE hFile = CreateFileW(
        filename,                
        GENERIC_READ,            
        FILE_SHARE_READ,         
        NULL,                    
        OPEN_EXISTING,           
        FILE_ATTRIBUTE_NORMAL,   
        NULL                     
    );

    if (hFile == NULL) {
        wcscpy(wsr_toastbuffer, L"Could not open file for reading.");
        TIRCriticalError(L"FILE OPEN ERROR");
        return;
    }

    uint32_t byteSize = GetFileSize(hFile, NULL),
             chunkSz  = byteSize / (MT_MAX_BODY * sizeof(wchar_t)),
             remSize  = byteSize % (MT_MAX_BODY * sizeof(wchar_t));

    

    /* Send begin message */
    message_t *begin = msg_makeraw();
    begin = msg_setType(begin, INFO_DATA_BEGIN);
    begin = msg_addbody(begin, (wchar_t*)PathFindFileNameW(filename), MIN(wcslen(filename), MT_MAX_BODY));    
    send(msock, (char*)begin, sizeof(message_t), 0);
    free(begin);

    /* Transfer */
    DWORD bytesRead;
    for (uint32_t ch = 0; ch < chunkSz; ch++) {
        message_t *msg = msg_makeraw();
        msg = msg_setType(msg, INFO_DATA);

        uint64_t tmpck = ((uint64_t)chunkSz << 32);
        tmpck |= ch;
        swprintf(wsr_toastbuffer, TOAST_LENGTH, L"DEBUG: BYTE=%u CHUNK=%u REM=%u UP=%08x LW=%08x\n", byteSize, chunkSz, remSize, (tmpck >> 32), (tmpck & 0xFFFFFFFF));
        msg = msg_setChunk(msg, tmpck);

        if (!ReadFile(hFile, msg->wsr_body, MT_MAX_BODY * sizeof(wchar_t), &bytesRead, NULL)) {
            msg = msg_setType(msg, INFO_DATA_ERROR);
        }

        send(msock, (char*)msg, sizeof(message_t), 0);
        free(msg);
    }
    message_t *end = msg_makeraw();
    end = msg_setType(end, INFO_DATA_END);
    end = msg_setArgument(end, remSize);
    if (!ReadFile(hFile, end->wsr_body, remSize, &bytesRead, NULL)) {
        end = msg_setType(end, INFO_DATA_ERROR);
    }
    send(msock, (char*)end, sizeof(message_t), 0);
    free(end);

    CloseHandle(hFile);
}