
#ifndef NXTERM_H_
#define NXTERM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <switch.h>

#define NXTERMHUD_ELEMENT_BUFSIZE 38
#define NXTERM_LINE_COUNT 39
#define NXTERM_LINE_BUFSIZE 80

extern bool _nxterm_cAllocated;
extern char _nxterm_lines[NXTERM_LINE_COUNT][NXTERM_LINE_BUFSIZE + 1];
extern int  _nxterm_cRow;
extern int  _nxterm_cCol;

typedef struct NXTermHUD {
	char tl_text[NXTERMHUD_ELEMENT_BUFSIZE]; // top left
	char tr_text[NXTERMHUD_ELEMENT_BUFSIZE]; // top right
	char bl_text[NXTERMHUD_ELEMENT_BUFSIZE]; // bottom left
	char br_text[NXTERMHUD_ELEMENT_BUFSIZE]; // bottom right
} NXTHUD;
int  nxterm_hud_secure_copy(char* pText, char* content);

void prv_nxterm_set_cursor_pos(int row, int col);

void nxterm_init(PrintConsole* pPrintConsole);
void nxterm_free(void);

void nxterm_hud_update(struct NXTermHUD* pNXTermHUD);

void nxterm_vc_clear(void);
void nxterm_vc_clear_row(int row, bool update_console);
void nxterm_vc_printf(const char* fmt, ...);

#endif
