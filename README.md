# nxterm

<img src="https://github.com/h4zesec/nxterm/blob/main/imgs/emulator_screenshot.png?raw=true">

`nxterm` is a lightweight terminal-style console utility for Nintendo Switch homebrew applications using **libnx**.

**Note**: nxterm was only tested on a Nintendo Switch OLED!

It provides a simple virtual console with:

* Cursor positioning
* Scrolling terminal output
* Line buffering
* ANSI color support
* A four-corner HUD
* Console clearing
* Formatted printing
* Console initialization and cleanup

<img src="https://github.com/h4zesec/nxterm/blob/main/imgs/on_real_hardware.png?raw=true">

## Requirements

* Nintendo Switch homebrew development environment
* [devkitPro](https://devkitpro.org/)
* [libnx](https://github.com/switchbrew/libnx)

The project includes `switch.h`, so it is intended to be built as part of a Nintendo Switch application using libnx.

## Files

```text
nxterm.h
nxterm.c
```

Include `nxterm.h` in your project and compile `nxterm.c` together with your application.

## Initialization

Initialize `nxterm` with a `PrintConsole` instance:

```c
PrintConsole console;

nxterm_init(&console);
```

or

```c
nxterm_init(NULL);
```

The console is initialized only once. Calling `nxterm_init()` again while the console is already initialized has no effect.

PrintConsole structure (https://switchbrew.github.io/libnx/console_8h_source.html):

```c
struct PrintConsole
{
    ConsoleFont font;          ///< Font of the console
    ConsoleRenderer* renderer; ///< Renderer of the console
 
    int cursorX;               ///< Current X location of the cursor (as a tile offset by default)
    int cursorY;               ///< Current Y location of the cursor (as a tile offset by default)
 
    int prevCursorX;           ///< Internal state
    int prevCursorY;           ///< Internal state
 
    int consoleWidth;          ///< Width of the console hardware layer in characters
    int consoleHeight;         ///< Height of the console hardware layer in characters
 
    int windowX;               ///< Window X location in characters
    int windowY;               ///< Window Y location in characters
    int windowWidth;           ///< Window width in characters
    int windowHeight;          ///< Window height in characters
 
    int tabSize;               ///< Size of a tab
    u16 fg;                    ///< Foreground color
    u16 bg;                    ///< Background color
    int flags;                 ///< Reverse/bright flags
 
    bool consoleInitialised;   ///< True if the console is initialized
};
```

When the application is finished with the terminal:

```c
nxterm_free();
```

This resets the internal cursor state and calls `consoleExit()`.

## Virtual Console

The virtual console uses rows `4` through `42` for normal terminal output.

Initialize the virtual console with:

```c
nxterm_vc_clear();
```

This clears the terminal area and places the cursor at the beginning of the output region.

### Printing

Formatted output can be written with:

```c
nxterm_vc_printf("Hello, world!\n");
nxterm_vc_printf("Value: %d\n", value);
```

`nxterm_vc_printf()` supports standard `printf`-style formatting.

The terminal maintains an internal buffer for each displayed line. When the bottom row is reached, the existing lines are shifted upward and the newest output remains at the bottom.

### Cursor Position

The virtual cursor can be moved with:

```c
nxterm_vc_goto(10, 5);
```

The position is stored internally and subsequent output continues from that location.

### Clearing a Row

A single console row can be cleared with:

```c
nxterm_vc_clear_row(10, true);
```

The second parameter determines whether `consoleUpdate(NULL)` is called immediately.

For example:

```c
nxterm_vc_clear_row(10, false);
consoleUpdate(NULL);
```

## HUD

`nxterm` provides four HUD elements:

* Top-left
* Top-right
* Bottom-left
* Bottom-right

They are represented by `NXTermHUD`:

```c
NXTHUD hud = {
    .tl_text = "Title",
    .tr_text = "Status",
    .bl_text = "Bottom Left",
    .br_text = "v1.0"
};
```

Update the HUD with:

```c
nxterm_hud_update(&hud);
```

The HUD is displayed independently from the main virtual console output.

The top-left and top-right elements are displayed on row `2`, while the bottom-left and bottom-right elements are displayed on row `44`.

The right-aligned elements are positioned according to their visible text length.

## HUD String Copy

For safely copying text into a HUD element, use:

```c
nxterm_hud_secure_copy(hud.tl_text, "My Application");
```

The function uses `snprintf()` with `NXTERMHUD_ELEMENT_BUFSIZE` as the maximum size.

## ANSI Colors

The following color definitions are provided:

```c
NXBLACK
NXRED
NXGREEN
NXYELLOW
NXBLUE
NXMAGENTA
NXCYAN
NXWHITE
```

They can be used directly with `nxterm_vc_printf()`:

```c
nxterm_vc_printf(NXGREEN "Success" NXWHITE "\n");
nxterm_vc_printf(NXRED "Error" NXWHITE "\n");
```

ANSI escape sequences are preserved in the internal line buffer so that scrolling does not lose the associated color formatting.

## Complete Example

```c
#include <switch.h>
#include "nxterm.h"

#define vcprintf(...) nxterm_vc_printf(__VA_ARGS__)

int main(int argc, char* argv[]) {
    nxterm_init(NULL);
    nxterm_vc_clear();

    // only set the *_text values like this if you know you wont cause a bufferoverflow.
    // use nxterm_hud_secure_copy to be safe
    NXTHUD hud = {
        .tl_text = "nxterm",
        .tr_text = NXGREEN "READY" NXWHITE,
        .bl_text = "Nintendo Switch",
        .br_text = NXTERM_VERSION
    };

    nxterm_hud_update(&hud);

    vcprintf(NXCYAN "nxterm example\n" NXWHITE);
    vcprintf("Virtual console initialized.\n");
    vcprintf("Value: %d\n", 123);

    while (appletMainLoop());

    nxterm_free();

    return 0;
}
```

## API

### `nxterm_init()`

```c
void nxterm_init(PrintConsole* pPrintConsole);
```

Initializes the console.

### `nxterm_free()`

```c
void nxterm_free(void);
```

Releases the console and resets the internal cursor state.

### `nxterm_vc_printf()`

```c
void nxterm_vc_printf(const char* fmt, ...);
```

Prints formatted text to the virtual console.

### `nxterm_vc_goto()`

```c
void nxterm_vc_goto(int row, int col);
```

Moves the virtual console cursor.

### `nxterm_vc_clear()`

```c
void nxterm_vc_clear(void);
```

Clears the virtual console output area.

### `nxterm_vc_clear_row()`

```c
void nxterm_vc_clear_row(int row, bool update_console);
```

Clears a specific console row.

### `nxterm_hud_update()`

```c
void nxterm_hud_update(struct NXTermHUD* pNXTermHUD);
```

Updates all four HUD elements.

### `nxterm_hud_secure_copy()`

```c
int nxterm_hud_secure_copy(char* pText, char* content);
```

Copies a string into a HUD buffer using the predefined HUD buffer size. Use this to avoid bufferoverflows.

### `prv_nxterm_set_cursor_pos()`

```c
void prv_nxterm_set_cursor_pos(int row, int col);
```

Sets the physical console cursor position using an ANSI cursor-position escape sequence.

This function is exposed by the header but is intended as an internal helper.

## Configuration

The main configuration values are defined in `nxterm.h`:

```c
#define NXTERM_VERSION "nxterm v1.00"

#define NXTERMHUD_ELEMENT_BUFSIZE 38
#define NXTERM_LINE_COUNT 39
#define NXTERM_LINE_BUFSIZE 512
```

### `NXTERMHUD_ELEMENT_BUFSIZE`

Maximum storage size for each HUD element.

### `NXTERM_LINE_COUNT`

Number of internally buffered virtual console lines.

### `NXTERM_LINE_BUFSIZE`

Maximum buffer size for each stored terminal line.

## Notes

`nxterm` is designed around the Nintendo Switch libnx console system and uses ANSI escape sequences for cursor positioning and colors.

The virtual console occupies rows `4` through `42`, leaving dedicated space for the HUD.

The implementation automatically scrolls when output reaches the bottom of the virtual console.
