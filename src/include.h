#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wchar.h>

#include <time.h>
#include <conio.h>
#include <assert.h>
#include <io.h>

#define WIN32_LEAN_AND_MEAN
#include <locale.h>
#include <shlwapi.h>
#include <ws2tcpip.h>
#include <shobjidl.h>
#include <winsock2.h>
#include <windows.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
void TIRCriticalError(wchar_t *text);