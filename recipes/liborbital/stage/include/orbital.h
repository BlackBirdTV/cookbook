/* C bindings to the Orbital client library */

#ifndef _ORBITAL_H
#define _ORBITAL_H

#include <stdbool.h>

#define ORB_KEY_0 11

#define ORB_KEY_1 2

#define ORB_KEY_2 3

#define ORB_KEY_3 4

#define ORB_KEY_4 5

#define ORB_KEY_5 6

#define ORB_KEY_6 7

#define ORB_KEY_7 8

#define ORB_KEY_8 9

#define ORB_KEY_9 10

#define ORB_KEY_A 30

#define ORB_KEY_ALT 56

#define ORB_KEY_B 48

#define ORB_KEY_BACKSLASH 43

#define ORB_KEY_BKSP 14

#define ORB_KEY_BRACE_CLOSE 27

#define ORB_KEY_BRACE_OPEN 26

#define ORB_KEY_C 46

#define ORB_KEY_CAPS 58

#define ORB_KEY_COMMA 51

#define ORB_KEY_CTRL 29

#define ORB_KEY_D 32

#define ORB_KEY_DEL 83

#define ORB_KEY_DOWN 80

#define ORB_KEY_E 18

#define ORB_KEY_END 79

#define ORB_KEY_ENTER 28

#define ORB_KEY_EQUALS 13

#define ORB_KEY_ESC 1

#define ORB_KEY_F 33

#define ORB_KEY_F1 59

#define ORB_KEY_F10 68

#define ORB_KEY_F11 87

#define ORB_KEY_F12 88

#define ORB_KEY_F2 60

#define ORB_KEY_F3 61

#define ORB_KEY_F4 62

#define ORB_KEY_F5 63

#define ORB_KEY_F6 64

#define ORB_KEY_F7 65

#define ORB_KEY_F8 66

#define ORB_KEY_F9 67

#define ORB_KEY_G 34

#define ORB_KEY_H 35

#define ORB_KEY_HOME 71

#define ORB_KEY_I 23

#define ORB_KEY_INSERT 82

#define ORB_KEY_J 36

#define ORB_KEY_K 37

#define ORB_KEY_L 38

#define ORB_KEY_LEFT 75

#define ORB_KEY_LEFT_SHIFT 42

#define ORB_KEY_M 50

#define ORB_KEY_MINUS 12

#define ORB_KEY_N 49

#define ORB_KEY_O 24

#define ORB_KEY_P 25

#define ORB_KEY_PERIOD 52

#define ORB_KEY_PGDN 81

#define ORB_KEY_PGUP 73

#define ORB_KEY_Q 16

#define ORB_KEY_QUOTE 40

#define ORB_KEY_R 19

#define ORB_KEY_RIGHT 77

#define ORB_KEY_RIGHT_SHIFT 54

#define ORB_KEY_S 31

#define ORB_KEY_SCROLL 70

#define ORB_KEY_SEMICOLON 39

#define ORB_KEY_SLASH 53

#define ORB_KEY_SPACE 57

#define ORB_KEY_T 20

#define ORB_KEY_TAB 15

#define ORB_KEY_TICK 41

#define ORB_KEY_U 22

#define ORB_KEY_UP 72

#define ORB_KEY_V 47

#define ORB_KEY_W 17

#define ORB_KEY_X 45

#define ORB_KEY_Y 21

#define ORB_KEY_Z 44

#define ORB_WINDOW_ASYNC 1

#define ORB_WINDOW_BACK 2

#define ORB_WINDOW_FRONT 4

#define ORB_WINDOW_RESIZABLE 8

#define ORB_WINDOW_UNCLOSABLE 16

typedef enum {
  OrbEventOption_Key,
  OrbEventOption_TextInput,
  OrbEventOption_Mouse,
  OrbEventOption_MouseRelative,
  OrbEventOption_Button,
  OrbEventOption_Scroll,
  OrbEventOption_Quit,
  OrbEventOption_Focus,
  OrbEventOption_Move,
  OrbEventOption_Resize,
  OrbEventOption_Screen,
  OrbEventOption_Clipboard,
  OrbEventOption_ClipboardUpdate,
  OrbEventOption_Drop,
  OrbEventOption_Unknown,
  OrbEventOption_None,
} OrbEventOption_Tag;

typedef struct {
  uint32_t character;
  uint8_t scancode;
  bool pressed;
} OrbEventOption_Key_Body;

typedef struct {
  uint32_t character;
} OrbEventOption_TextInput_Body;

typedef struct {
  int32_t x;
  int32_t y;
} OrbEventOption_Mouse_Body;

typedef struct {
  int32_t dx;
  int32_t dy;
} OrbEventOption_MouseRelative_Body;

typedef struct {
  bool left;
  bool middle;
  bool right;
} OrbEventOption_Button_Body;

typedef struct {
  int32_t x;
  int32_t y;
} OrbEventOption_Scroll_Body;

typedef struct {

} OrbEventOption_Quit_Body;

typedef struct {
  bool focused;
} OrbEventOption_Focus_Body;

typedef struct {
  int32_t x;
  int32_t y;
} OrbEventOption_Move_Body;

typedef struct {
  uint32_t width;
  uint32_t height;
} OrbEventOption_Resize_Body;

typedef struct {
  uint32_t width;
  uint32_t height;
} OrbEventOption_Screen_Body;

typedef struct {
  uint8_t kind;
  uintptr_t size;
} OrbEventOption_Clipboard_Body;

typedef struct {
  uint8_t kind;
} OrbEventOption_Drop_Body;

typedef struct {
  int64_t code;
  int64_t a;
  int64_t b;
} OrbEventOption_Unknown_Body;

typedef struct {
  OrbEventOption_Tag tag;
  union {
    OrbEventOption_Key_Body key;
    OrbEventOption_TextInput_Body text_input;
    OrbEventOption_Mouse_Body mouse;
    OrbEventOption_MouseRelative_Body mouse_relative;
    OrbEventOption_Button_Body button;
    OrbEventOption_Scroll_Body scroll;
    OrbEventOption_Quit_Body quit;
    OrbEventOption_Focus_Body focus;
    OrbEventOption_Move_Body move;
    OrbEventOption_Resize_Body resize;
    OrbEventOption_Screen_Body screen;
    OrbEventOption_Clipboard_Body clipboard;
    OrbEventOption_Drop_Body drop;
    OrbEventOption_Unknown_Body unknown;
  };
} OrbEventOption;

#ifdef __cplusplus
extern "C" {
#endif

uint32_t orb_display_height(void);

uint32_t orb_display_width(void);

void orb_events_destroy(void *event_iter);

OrbEventOption orb_events_next(void *event_iter);

uint32_t *orb_window_data(void *window);

void orb_window_destroy(void *window);

void *orb_window_events(void *window);

uint32_t orb_window_height(void *window);

void *orb_window_new(int32_t x, int32_t y, uint32_t w, uint32_t h, const char *title);

void *orb_window_new_flags(int32_t x,
                           int32_t y,
                           uint32_t w,
                           uint32_t h,
                           const char *title,
                           uint32_t flags);

void orb_window_set_mouse_cursor(void *window, bool cursor);

void orb_window_set_mouse_grab(void *window, bool grab);

void orb_window_set_mouse_relative(void *window, bool relative);

void orb_window_set_pos(void *window, int32_t x, int32_t y);

void orb_window_set_size(void *window, uint32_t w, uint32_t h);

void orb_window_set_title(void *window, const char *title);

void orb_window_sync(void *window);

uint32_t orb_window_width(void *window);

int32_t orb_window_x(void *window);

int32_t orb_window_y(void *window);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _ORBITAL_H */
