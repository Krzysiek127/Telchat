#include "message.h"

extern HANDLE hOutput;

extern wchar_t wsr_current_user[MT_MAX_USERNAME],
               wsr_toastbuffer[TOAST_LENGTH];


int mesprintf(message_t *msg) {
    if (msg == NULL)
        return -1;
    
    //printf("%ls> %ls\n", message_queue[i]->wsr_username, message_queue[i]->wsr_body);

    if (*(msg->wsr_address)) {
        SetConsoleTextAttribute(hOutput, FOREGROUND_RED);
        printf("PRIV ");
    }
    SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN);
    printf("%ls", msg->wsr_username);

    SetConsoleTextAttribute(hOutput, FOREGROUND_INTENSITY);
    printf("> ");

    SetConsoleTextAttribute(hOutput, FOREGROUND_DEFAULT);

    BOOL shouldRender = TRUE;
    WORD wTextAttr = 0;
    wchar_t *privUser, *privUserPers;

    for (wchar_t *wch = msg->wsr_body; *wch; wch++) {
        switch (*wch) {
            case L'$':
                ++wch;
                switch (*wch) {
                    case L'$':
                        goto print;
                    case L'!':
                        SetConsoleTextAttribute(hOutput, FOREGROUND_DEFAULT);
                        wTextAttr = 0;
                        shouldRender = TRUE;
                        continue;
                    case L'P':
                        printf("\a");
                        break;

                    case L'>':
                        wTextAttr |= FOREGROUND_INTENSITY;
                        break;
                    case L'<':
                        wTextAttr |= BACKGROUND_INTENSITY;
                        break;
                    
                    /* Foregrounds */
                    case L'r':
                        wTextAttr |= FOREGROUND_RED;
                        break;
                    case L'g':
                        wTextAttr |= FOREGROUND_GREEN;
                        break;
                    case L'b':
                        wTextAttr |= FOREGROUND_BLUE;
                        break;

                    /* Backgrounds */
                    case L'R':
                        wTextAttr |= BACKGROUND_RED;
                        break;
                    case L'G':
                        wTextAttr |= BACKGROUND_GREEN;
                        break;
                    case L'B':
                        wTextAttr |= BACKGROUND_BLUE;
                        break;
                    default:
                        goto print;

                }
                SetConsoleTextAttribute(hOutput, wTextAttr == 0 ? FOREGROUND_DEFAULT : wTextAttr);
                break;
            default:
            print:
                if (shouldRender)
                    printf("%lc", *wch);
                break;
        }
    }
    
    SetConsoleTextAttribute(hOutput, FOREGROUND_DEFAULT);
    printf("\n");
    return 0;
}