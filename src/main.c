#include "message.h"

#define MSG_PER_SCREEN  12

#define TCP_DATA_PORT   2005
#define UDP_BRDC_PORT   2007


wchar_t wsr_current_user[MT_MAX_USERNAME],
        wsr_linebuffer[MT_MAX_BODY],
        wsr_toastbuffer[TOAST_LENGTH];

uint32_t u_lbix = 0;    /* u_linebuffer_index */

message_t *message_queue[MSG_PER_SCREEN];

/* Windows stuff */
HANDLE hOutput;
WSADATA wsaData;

CONSOLE_SCREEN_BUFFER_INFO s;
static DWORD written;



void clear_terminal(void) {
    FillConsoleOutputCharacter(hOutput, ' ', s.dwSize.X * s.dwSize.Y, (COORD){0, 0}, &written);
    FillConsoleOutputAttribute(hOutput, s.wAttributes, s.dwSize.X * s.dwSize.Y, (COORD){0, 0}, &written);

    FillConsoleOutputAttribute(hOutput, BACKGROUND_INTENSITY, s.dwSize.X, (COORD){0, 1}, &written);

    SetConsoleCursorPosition(hOutput, (COORD){0, 0});
}

void cursor(WINBOOL state) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOutput, &cursorInfo);
    cursorInfo.bVisible = state;
    SetConsoleCursorInfo(hOutput, &cursorInfo);
}

void TIRCriticalError(wchar_t *text)
{
    if (hOutput)
        clear_terminal();
    
    MessageBoxW(NULL, text, L"TIRC Error", MB_ICONERROR);
    exit(1);
}

void scroll_queue(message_t *new) {
    free(message_queue[0]);
    for (int i = 0; i<MSG_PER_SCREEN; i++) {
        message_queue[i] = message_queue[i + 1];
    }
    message_queue[MSG_PER_SCREEN - 1] = new;
}

static message_t *connmsg;
int client(unsigned long ADDR, u_short PORT); // forward declaration
int main(int argc, char *argv[]) {
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hOutput, &s);
    clear_terminal();
    
    SetConsoleTitleW(L"Telchat Alpha 1.0");

    printf("\n\nTelthar Internet LAN Chat\nVersion Alpha 1.0, Protocol Version %u\n", PROTO_VERSION);
    srand(time(NULL));

    // TODO: Username set
    wcscpy(wsr_current_user, L"Krzysiek");

    setlocale( LC_ALL, ".UTF8" );

    /* Winsock init part */
    cursor(FALSE);
    if (WSAStartup( MAKEWORD(2, 2), &wsaData ) != NO_ERROR)
        TIRCriticalError(L"Initialization error");


    if (argc != 2)
        TIRCriticalError(L"Argument count error");
    
    struct hostent *host_entry = gethostbyname(argv[1]);
    if (host_entry == NULL)
        TIRCriticalError(L"Invalid hostname.");

    char *IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
    return client(inet_addr(IP), TCP_DATA_PORT);
}

wchar_t* OpenFileDialog()
{
    HRESULT hr;
    IFileDialog* pFileDialog = NULL;
    wchar_t* pszFilePath = NULL;

    // Initialize COM library
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        return NULL; // COM initialization failed
    }

    // Create the File Open Dialog object
    hr = CoCreateInstance(&CLSID_FileOpenDialog, NULL, CLSCTX_ALL, &IID_IFileDialog, (void**)&pFileDialog);
    if (SUCCEEDED(hr))
    {
        // Set the file types to *.*
        COMDLG_FILTERSPEC fileTypes[] = {
            { L"All Files", L"*.*" }
        };

        // Set file dialog options
        DWORD dwFlags;
        hr = pFileDialog->lpVtbl->GetOptions(pFileDialog, &dwFlags);
        if (SUCCEEDED(hr))
        {
            hr = pFileDialog->lpVtbl->SetOptions(pFileDialog, dwFlags | FOS_FORCEFILESYSTEM);
        }

        // Set the file type filter
        pFileDialog->lpVtbl->SetFileTypes(pFileDialog, ARRAYSIZE(fileTypes), fileTypes);

        // Show the dialog
        hr = pFileDialog->lpVtbl->Show(pFileDialog, NULL);
        if (SUCCEEDED(hr))
        {
            // Get the result
            IShellItem* pItem = NULL;
            hr = pFileDialog->lpVtbl->GetResult(pFileDialog, &pItem);
            if (SUCCEEDED(hr))
            {
                PWSTR tempFilePath = NULL;
                hr = pItem->lpVtbl->GetDisplayName(pItem, SIGDN_FILESYSPATH, &tempFilePath);
                if (SUCCEEDED(hr))
                {
                    // Allocate memory for the file path and copy the result
                    size_t len = wcslen(tempFilePath) + 1;
                    pszFilePath = (wchar_t*)malloc(len * sizeof(wchar_t));
                    if (pszFilePath)
                    {
                        wcscpy_s(pszFilePath, len, tempFilePath);
                    }
                    CoTaskMemFree(tempFilePath);
                }
                pItem->lpVtbl->Release(pItem);
            }
        }

        // Release the file dialog object
        pFileDialog->lpVtbl->Release(pFileDialog);
    }

    // Uninitialize COM
    CoUninitialize();

    return pszFilePath; // Returns the file path or NULL if failed
}

static BOOL shouldDownload = FALSE;
wchar_t *nd_wcstok(wchar_t *__restrict__ _Str, const wchar_t *__restrict__ _Delim, wchar_t **_Ptr) {
    wchar_t *cpy = calloc(wcslen(_Str), sizeof(wchar_t));
    wcscpy(cpy, _Str);
    return wcstok(cpy, _Delim, _Ptr);
}

int client(unsigned long ADDR, u_short PORT) {
    SOCKET mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainSocket == INVALID_SOCKET) {
        WSACleanup();
        TIRCriticalError(L"Error creating socket.");
    }

    struct sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = ADDR;
    service.sin_port = htons(PORT);

    if ( connect(mainSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        closesocket(mainSocket);
        WSACleanup();
        TIRCriticalError(L"Could not make a connection to host");
    }

    u_long mode = 1;
    ioctlsocket(mainSocket, FIONBIO, &mode);




    connmsg = msg_makeraw();
    connmsg = msg_setType(connmsg, INFO_CONNECT);
    send(mainSocket, (char*)connmsg, sizeof(message_t), 0);
    free(connmsg);
    
    wchar_t wc_get;
    while (1) {
        
        /* Key input segment */
        if (_kbhit()) {
            wc_get = (wchar_t) _getwch();
            
            /* Add escape codes */

            switch (wc_get) {
            case L'\r':
                if (wsr_linebuffer[0] == 0)
                    break;
                
                if (!wcscmp(wsr_linebuffer, L"/send")) {
                    wchar_t *fpth = OpenFileDialog();
                    if (fpth == NULL) {
                        MessageBoxW(NULL, L"No file selected or operation failed.", L"File open error", 0);
                        free(fpth);
                        break;
                    }

                    msg_sendfile(mainSocket, fpth);
                    free(fpth);
                    wmemset(wsr_linebuffer, 0, MT_MAX_BODY);
                    clear_terminal();
                    break;
                }

                if (!wcscmp(wsr_linebuffer, L"/recv")) {
                    shouldDownload = TRUE;
                    swprintf(wsr_toastbuffer, TOAST_LENGTH, L"Ready to receive file.");
                    wmemset(wsr_linebuffer, 0, MT_MAX_BODY);
                    clear_terminal();
                    break;
                }

                message_t *sendmsg = msg_makeraw(); //make_text_message(wsr_linebuffer, u_lbix);

                int offset = 0;
                wchar_t *argslcmdPer;
                wchar_t *argslcmd = nd_wcstok(wsr_linebuffer, L" ", &argslcmdPer);
                if (argslcmd[0] == L'/' && argslcmd[1] == L'p' && argslcmd[2] == L'+') {    //  `/p+User`
                    argslcmd += 3;
                    offset += 4;    // 4 to remove the space after user e.g. /p+User[ ]Message...

                    sendmsg = msg_addRecp(sendmsg, argslcmd);
                    offset += wcslen(argslcmd);
                }

                sendmsg = msg_addbody(sendmsg, wsr_linebuffer + offset, u_lbix);

                scroll_queue(sendmsg);
                send(mainSocket, (char*)sendmsg, sizeof(message_t), 0);

                u_lbix = 0;
                wmemset(wsr_linebuffer, 0, MT_MAX_BODY);

                clear_terminal();

                // no free(sendmsg) because it goes to scroll_queue()
                break;
            case L'\b':
                if (u_lbix)
                    wsr_linebuffer[--u_lbix] = 0;
                clear_terminal();
                break;
            case 0x1b:  /* 'esc' key */
                goto exit_loop;
            default:
                if (u_lbix < MT_MAX_BODY)
                    wsr_linebuffer[u_lbix++] = wc_get;
                break;
            }
        }

        SetConsoleCursorPosition(hOutput, (COORD){0, 0});
        printf("%ls@ %ls\n%ls", wsr_current_user, wsr_linebuffer, wsr_toastbuffer);
        FillConsoleOutputAttribute(hOutput, BACKGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED, s.dwSize.X, (COORD){0, 1}, &written);

        int ofs = 3;
        if (u_lbix >= s.dwSize.X)
            ofs++;
        

        /* Receiving */
        message_t *rcvmsg = calloc(1, sizeof(message_t));   /* Should be freed upon scroll_queue() end (theoretically) */
        int rcv = recv(mainSocket, (char*)rcvmsg, sizeof(message_t), 0);

        if (rcv < 0) {
            switch (WSAGetLastError())
            {
            case WSAEWOULDBLOCK:
                Sleep(100);
                goto render_segment;
            case WSAECONNRESET:
                TIRCriticalError(L"An existing connection was forcibly closed by the remote host.");
            case WSAECONNREFUSED:
                TIRCriticalError(L"No connection could be made because the target computer actively refused it.");
            default:
                TIRCriticalError(L"Unknown error");
            }
        }

        wchar_t *wcs_addr = rcvmsg->wsr_address;
        if (*wcs_addr != 0 && wcscmp(wcs_addr, wsr_current_user)) {
            free(rcvmsg);
            continue;
        }

        /* Here we've received new message, we should process what type is it */
        //uint16_t mflag = rcvmsg->us_flags;
        struct tm *time_info = localtime(&rcvmsg->tm_timestamp);
        HANDLE hRecvf;

        switch (rcvmsg->uc_type) {
        case INFO_TEXT:
            scroll_queue(rcvmsg);
            clear_terminal();
            break;
        
        case INFO_DATA_BEGIN:
            if (shouldDownload) {
                hRecvf = CreateFileW(
                    rcvmsg->wsr_body,                
                    GENERIC_WRITE,           // Open for writing
                    0,                       // Do not share
                    NULL,                    // Default security
                    CREATE_ALWAYS,           // Overwrite if file exists
                    FILE_ATTRIBUTE_NORMAL,   // Normal file
                    NULL                     // No template
                );

                if (hRecvf == INVALID_HANDLE_VALUE) {
                    MessageBoxW(NULL, L"Could not create file", L"File error", 0);
                    shouldDownload = FALSE;
                }
                    
            }
            goto end_of_render;
        case INFO_DATA:
            if (shouldDownload && hRecvf != INVALID_HANDLE_VALUE)
                WriteFile(hRecvf, rcvmsg->wsr_body, MT_MAX_BODY * sizeof(wchar_t), &written, NULL);
            goto end_of_render;
        case INFO_DATA_END:
            if (shouldDownload && hRecvf != INVALID_HANDLE_VALUE) {
                WriteFile(hRecvf, rcvmsg->wsr_body, rcvmsg->u_arg, &written, NULL);
                CloseHandle(hRecvf);
                shouldDownload = FALSE;
                swprintf(wsr_toastbuffer, TOAST_LENGTH, L"File received");
            }
            goto end_of_render;
        case INFO_DATA_ERROR:
            swprintf(wsr_toastbuffer, TOAST_LENGTH, L"Transfer error.");
            if (hRecvf != INVALID_HANDLE_VALUE)
                CloseHandle(hRecvf);
            goto end_of_render;
        
        case INFO_CONNECT:
            swprintf(wsr_toastbuffer, TOAST_LENGTH, L"[%u:%u] %ls connected!", time_info->tm_hour, time_info->tm_min, rcvmsg->wsr_username);
            clear_terminal();
            goto end_of_render;
        case INFO_DXCONNECT:
            swprintf(wsr_toastbuffer, TOAST_LENGTH, L"[%u:%u] %ls left!", time_info->tm_hour, time_info->tm_min, rcvmsg->wsr_username);
            clear_terminal();
            goto end_of_render;
        case INFO_BROADCAST:
            swprintf(wsr_toastbuffer, TOAST_LENGTH, L"[ADMIN] %ls", rcvmsg->wsr_body);
            clear_terminal();
            goto end_of_render;
        
        default:
            TIRCriticalError(L"Received message with invalid type.");
        }

        render_segment:
        /* Rendering segment */
        for (int i = 0; i < MSG_PER_SCREEN; i++) {
            SetConsoleCursorPosition(hOutput, (COORD){0, ofs + (i * 2)});
            mesprintf(message_queue[i]);
        } end_of_render:

        if (rcvmsg->uc_type != INFO_TEXT)
            free(rcvmsg);
    }
exit_loop:
    message_t *connmsg = msg_makeraw();
    connmsg = msg_setType(connmsg, INFO_DXCONNECT);

    send(mainSocket, (char*)connmsg, sizeof(message_t), 0);
    free(connmsg);

    closesocket(mainSocket);
    WSACleanup();

    cursor(TRUE);
    return 0;
}