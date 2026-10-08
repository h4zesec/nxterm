
#ifndef NXTERM_H_
#define NXTERM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <switch.h>

#define NXTERM_VERSION "nxterm v1.00"

#define NXTERMHUD_ELEMENT_BUFSIZE 38
#define NXTERM_LINE_COUNT 39
#define NXTERM_LINE_BUFSIZE 512

extern bool _nxterm_cAllocated;
extern char _nxterm_lines[NXTERM_LINE_COUNT][NXTERM_LINE_BUFSIZE + 1];
extern int  _nxterm_cRow;
extern int  _nxterm_cCol;

typedef struct NXTermHUD {
	char tl_text[NXTERM_LINE_BUFSIZE]; // top left
	char tr_text[NXTERM_LINE_BUFSIZE]; // top right
	char bl_text[NXTERM_LINE_BUFSIZE]; // bottom left
	char br_text[NXTERM_LINE_BUFSIZE]; // bottom right
} NXTHUD;
int  nxterm_hud_secure_copy(char* pText, char* content);

void prv_nxterm_set_cursor_pos(int row, int col);

void nxterm_init(PrintConsole* pPrintConsole);
void nxterm_free(void);

void nxterm_hud_update(struct NXTermHUD* pNXTermHUD);

void nxterm_vc_clear(void);
void nxterm_vc_clear_row(int row, bool update_console);
void nxterm_vc_printf(const char* fmt, ...);
void nxterm_vc_goto(int	row, int col);

// use these color defs to avoid any issues
#define NXBLACK   "\e[0;30m"
#define NXRED     "\e[0;31m"
#define NXGREEN   "\e[0;32m"
#define NXYELLOW  "\e[0;33m"
#define NXBLUE    "\e[0;34m"
#define NXMAGENTA "\e[0;35m"
#define NXCYAN    "\e[0;36m"
#define NXWHITE   "\e[0;37m"

#endif
