
#ifndef NXTERM_H_
#define NXTERM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <switch.h>

#define NXTERMHUD_ELEMENT_BUFSIZE 38

extern bool _nxterm_cAllocated;
extern int  _nxterm_cRow;
extern int  _nxterm_cCol;

typedef struct NXTermHUD {
	char tl_text[NXTERMHUD_ELEMENT_BUFSIZE]; // top left
	char tr_text[NXTERMHUD_ELEMENT_BUFSIZE]; // top right
	char bl_text[NXTERMHUD_ELEMENT_BUFSIZE]; // bottom left
	char br_text[NXTERMHUD_ELEMENT_BUFSIZE]; // bottom right
};

void prv_nxterm_set_cursor_pos(int row, int col);

void nxterm_init(PrintConsole* pPrintConsole);
void nxterm_free(void);

void nxterm_update_hud(struct NXTermHUD* pNXTermHUD);

void nxterm_vc_clear(void);
void nxterm_vc_clear_row(int row, bool update_console);
void nxterm_vc_printf(const char* fmt, ...);

#endif
