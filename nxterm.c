
#include "nxterm.h"

bool _nxterm_cAllocated = false;
char _nxterm_lines[NXTERM_LINE_COUNT][NXTERM_LINE_BUFSIZE + 1];
int  _nxterm_cRow = 0;
int  _nxterm_cCol = 0;

void nxterm_init(PrintConsole* pPrintConsole) {
	if (_nxterm_cAllocated) return;

	consoleInit(pPrintConsole);
	_nxterm_cAllocated = true;
}

void nxterm_free(void) {
	if (!_nxterm_cAllocated) return;

	_nxterm_cRow = 0;
	_nxterm_cCol = 0;

	consoleExit(NULL);
	_nxterm_cAllocated = false;
}

void prv_nxterm_set_cursor_pos(int row, int col) {
	if (!_nxterm_cAllocated) return;

	printf("\x1b[%d:%dH", row, col);
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
	consoleUpdate(NULL);
}

void nxterm_vc_clear_row(int row, bool update_console) {
	if (!_nxterm_cAllocated) return;

	prv_nxterm_set_cursor_pos(row, 0);
	for (int i = 0; i <= 80; i++) printf(" ");
	
	prv_nxterm_set_cursor_pos(_nxterm_cRow, _nxterm_cCol); // restore cursor to its original position
	if (update_console) consoleUpdate(NULL);
}

void nxterm_vc_clear(void) {
	if (!_nxterm_cAllocated) return;

	for (int row = 4; row <= 42; row++) nxterm_vc_clear_row(row, false);
	consoleUpdate(NULL);
}

void nxterm_vc_printf(const char* fmt, ...) {
	if (!_nxterm_cAllocated) return;

	va_list args;
	va_start(args, fmt);

	char buf[512];
	vsnprintf(buf, sizeof(buf), fmt, args);

	va_end(args);

	for (int index = 0; buf[index] != '\0'; index++) {
		char c = buf[index];

		if (c == '\n' || _nxterm_cCol == 80) {
			_nxterm_cRow++;
			_nxterm_cCol = 0;

			if (_nxterm_cRow == 43) {
				for (int i = 0; i < 38; i++) {
					strcpy(_nxterm_lines[i], _nxterm_lines[i + 1]);
				}

				_nxterm_lines[38][0] = '\0';
				_nxterm_cRow = 42;

				for (int row = 4; row <= 42; row++) {
					printf("\x1b[%d;1H", row);

					printf("%-80s", _nxterm_lines[row - 4]);
				}
			}

			printf("\x1b[%d;1H", _nxterm_cRow + 1);
			continue;
		}

		if (c == '\r') {
			_nxterm_cCol = 0;
			printf("\r");
			continue;
		}

		int line = _nxterm_cRow - 4;
		if (line >= 0 && line < 39) {
			_nxterm_lines[line][_nxterm_cCol] = c;
			_nxterm_lines[line][_nxterm_cCol + 1] = '\0';
		}

		printf("%c", c);
		_nxterm_cCol++;
	}

	printf("\x1b[%d;%dH", _nxterm_cRow + 1, _nxterm_cCol + 1);

	consoleUpdate(NULL);
}
