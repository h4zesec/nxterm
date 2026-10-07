
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

	printf("\x1b[%d;%dH", row, col);
}

void nxterm_vc_goto(int row, int col) {
	if (!_nxterm_cAllocated) return;

	prv_nxterm_set_cursor_pos(row, col);
	_nxterm_cRow = row;
	_nxterm_cCol = col;
}

void nxterm_hud_update(struct NXTermHUD* pNXTermHUD) {
	if (!_nxterm_cAllocated) return;

	int len;

	len = 0;
	for (int i = 0; pNXTermHUD->tl_text[i] != '\0'; i++) {
		if (pNXTermHUD->tl_text[i] == '\e' && pNXTermHUD->tl_text[i + 1] == '[') {
			while (pNXTermHUD->tl_text[i] != 'm' && pNXTermHUD->tl_text[i] != '\0') i++;
			continue;
		}
		len++;
	}
	prv_nxterm_set_cursor_pos(2, 2);
	printf("%s", pNXTermHUD->tl_text);
	for (int i = len; i < 40; i++) printf(" ");

	len = 0;
	for (int i = 0; pNXTermHUD->bl_text[i] != '\0'; i++) {
		if (pNXTermHUD->bl_text[i] == '\e' && pNXTermHUD->bl_text[i + 1] == '[') {
			while (pNXTermHUD->bl_text[i] != 'm' && pNXTermHUD->bl_text[i] != '\0') i++;
			continue;
		}
		len++;
	}
	prv_nxterm_set_cursor_pos(44, 2);
	printf("%s", pNXTermHUD->bl_text);
	for (int i = len; i < 40; i++) printf(" ");

	len = 0;
	for (int i = 0; pNXTermHUD->tr_text[i] != '\0'; i++) {
		if (pNXTermHUD->tr_text[i] == '\e' && pNXTermHUD->tr_text[i + 1] == '[') {
			while (pNXTermHUD->tr_text[i] != 'm' && pNXTermHUD->tr_text[i] != '\0') i++;
			continue;
		}
		len++;
	}
	prv_nxterm_set_cursor_pos(2, 80 - len);
	printf("%s", pNXTermHUD->tr_text);

	len = 0;
	for (int i = 0; pNXTermHUD->br_text[i] != '\0'; i++) {
		if (pNXTermHUD->br_text[i] == '\e' && pNXTermHUD->br_text[i + 1] == '[') {
			while (pNXTermHUD->br_text[i] != 'm' && pNXTermHUD->br_text[i] != '\0') i++;
			continue;
		}
		len++;
	}
	prv_nxterm_set_cursor_pos(44, 80 - len);
	printf("%s", pNXTermHUD->br_text);

	prv_nxterm_set_cursor_pos(_nxterm_cRow, _nxterm_cCol + 2);
	consoleUpdate(NULL);
}

int nxterm_hud_secure_copy(char* pText, char* content) {
	return snprintf(pText, NXTERMHUD_ELEMENT_BUFSIZE, "%s", content);
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

	_nxterm_cRow = 4;
	_nxterm_cCol = 0;
	prv_nxterm_set_cursor_pos(_nxterm_cRow, 2);

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

		if (c == '\n' || _nxterm_cCol == 78) {
			if (_nxterm_cRow == 42) {
				for (int i = 0; i < 38; i++)
					strcpy(_nxterm_lines[i], _nxterm_lines[i + 1]);

				_nxterm_lines[38][0] = '\0';

				for (int row = 4; row <= 42; row++) {
					prv_nxterm_set_cursor_pos(row, 2);
					printf("%-78s ", _nxterm_lines[row - 4]);
					printf(NXWHITE);
				}

				_nxterm_cRow = 42;
				_nxterm_cCol = 0;

				prv_nxterm_set_cursor_pos(42, 2);
			}
			else {
				_nxterm_cRow++;
				_nxterm_cCol = 0;

				prv_nxterm_set_cursor_pos(_nxterm_cRow, 2);
			}

			continue;
		}

		if (c == '\r') {
			_nxterm_cCol = 0;
			prv_nxterm_set_cursor_pos(_nxterm_cRow, 2);
			continue;
		}

		int line = _nxterm_cRow - 4;

		if (line < 0 || line >= NXTERM_LINE_COUNT) continue;

		if (c == '\x1b' && buf[index + 1] == '[') {
			int start = index;
			index += 2;

			while (buf[index] != '\0') {
				char ansi_char = buf[index];

				if (ansi_char >= 0x40 && ansi_char <= 0x7E) break;

				index++;
			}

			if (buf[index] != '\0') {
				int len = index - start + 1;
				int current_len = strlen(_nxterm_lines[line]);

				if (current_len + len < NXTERM_LINE_BUFSIZE) {
					memcpy(&_nxterm_lines[line][current_len], &buf[start], len);
					_nxterm_lines[line][current_len + len] = '\0';
				}

				printf("%.*s", len, &buf[start]);
			}

			continue;
		}

		if (_nxterm_cCol < 78) {
			int current_len = strlen(_nxterm_lines[line]);

			if (current_len + 1 < NXTERM_LINE_BUFSIZE) {
				_nxterm_lines[line][current_len] = c;
				_nxterm_lines[line][current_len + 1] = '\0';
			}

			prv_nxterm_set_cursor_pos(_nxterm_cRow, _nxterm_cCol + 2);
			printf("%c", c);

			_nxterm_cCol++;
		}
	}

	prv_nxterm_set_cursor_pos(_nxterm_cRow, _nxterm_cCol + 2);

	consoleUpdate(NULL);
}
