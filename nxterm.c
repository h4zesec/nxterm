
#include "nxterm.h"

bool _nxterm_cAllocated = false;
int  _nxterm_cRow = 0;
int  _nxterm_cCol = 0;

void nxterm_init(PrintConsole* pPrintConsole) {
	if (_nxterm_cAllocated) return;

	consoleInit(pPrintConsole);
	_nxterm_cAllocated = true;
}

void nxterm_free(void) {
	if (!_nxterm_cAllocated) return;

	consoleExit(NULL);
	_nxterm_cAllocated = false;
}

void prv_nxterm_set_cursor_pos(int row, int col) {
	if (!_nxterm_cAllocated) return;

	printf("\x1b[%d;%dH", row, col);
}

void nxterm_update_hud(struct NXTermHUD* pNXTermHUD) {
	if (!_nxterm_cAllocated) return;

	prv_nxterm_set_cursor_pos(2, 2);
	printf("%s", pNXTermHUD->tl_text);
	for (int i = 0; i < NXTERMHUD_ELEMENT_BUFSIZE - strlen(pNXTermHUD->tl_text); i++) printf(" ");

	prv_nxterm_set_cursor_pos(44, 2);
	printf("%s", pNXTermHUD->bl_text);
	for (int i = 0; i < NXTERMHUD_ELEMENT_BUFSIZE - strlen(pNXTermHUD->bl_text); i++) printf(" ");


	prv_nxterm_set_cursor_pos(2, 40);
	for (int i = 0; i < NXTERMHUD_ELEMENT_BUFSIZE - strlen(pNXTermHUD->tr_text); i++) printf(" ");
	printf("%s", pNXTermHUD->tr_text);

	prv_nxterm_set_cursor_pos(44, 40);
	for (int i = 0; i < NXTERMHUD_ELEMENT_BUFSIZE - strlen(pNXTermHUD->br_text); i++) printf(" ");
	printf("%s", pNXTermHUD->br_text);

	prv_nxterm_set_cursor_pos(_nxterm_cRow, _nxterm_cCol); // restore cursor to its original position
	updateConsole(NULL);
}

void nxterm_vc_clear_row(int row, bool update_console) {
	if (!_nxterm_cAllocated) return;

	prv_nxterm_set_cursor_pos(row, 0);
	for (int i = 0; i <= 80; i++) printf(" ");
	
	prv_nxterm_set_cursor_pos(_nxterm_cRow, _nxterm_cCol); // restore cursor to its original position
	if (update_console) updateConsole(NULL);
}

void nxterm_vc_clear(void) {
	if (!_nxterm_cAllocated) return;

	for (int row = 4; row <= 42; row++) nxterm_vc_clear_row(row, false);
	updateConsole(NULL);
}

// TODO: Move lines up and create char** array for each line from the 4th to the 42nd line. Delete first
// item of array after the 42nd char* is filled and move every char* one down (42nd -> 41st, 41st -> 40, ...) and
// clear the 42nd char* for the new line that is going the printed.
  
void nxterm_vc_printf(const char* fmt, ...) {
	if (!_nxterm_cAllocated) return;

	va_list args;
	va_start(args, fmt);

	char buf[512];
	vsnprintf(buf, sizeof(buf), fmt, args);

	va_end(args);

	for (int index = 0; buf[index] != '\0'; index++) {
		char c = buf[index];
		if (c == '\n') {
			_nxterm_cRow++;
			_nxterm_cCol = 0;

			printf("\n");
			consoleUpdate(NULL);
			continue;
		}
		else if (c == '\r') _nxterm_cCol = 0;

		_nxterm_cCol++;
		printf("%c", c);
	}

	consoleUpdate(NULL);
}
