/*
 * Internals of the Terminal structure, for those other modules
 * which need to look inside it. It would be nice if this could be
 * folded back into terminal.c in future, with an abstraction layer
 * to handle everything that other modules need to know about it;
 * but for the moment, this will do.
 */

#ifndef PUTTY_TERMINAL_H
#define PUTTY_TERMINAL_H

#include "tree234.h"

struct beeptime {
    struct beeptime *next;
    unsigned long ticks;
};

typedef struct {
    int y, x;
} pos;

struct terminal_tag {

    int compatibility_level;

    tree234 *scrollback;	       /* lines scrolled off top of screen */
    tree234 *screen;		       /* lines on primary screen */
    tree234 *alt_screen;	       /* lines on alternate screen */
    int disptop;		       /* distance scrolled back (0 or -ve) */

    unsigned long *cpos;	       /* cursor position (convenience) */

    unsigned long *disptext;	       /* buffer of text on real screen */
    unsigned long *dispcurs;	       /* location of cursor on real screen */
    unsigned long curstype;	       /* type of cursor on real screen */

#define VBELL_TIMEOUT (TICKSPERSEC/10) /* visual bell lasts 1/10 sec */

    struct beeptime *beephead, *beeptail;
    int nbeeps;
    int beep_overloaded;
    long lastbeep;

#define TSIZE (sizeof(unsigned long))
#define fix_cpos do { \
    term->cpos = lineptr(term->curs.y) + term->curs.x; \
} while(0)

    unsigned long curr_attr, save_attr;
    unsigned long erase_char;

    bufchain inbuf;		       /* terminal input buffer */
    pos curs;			       /* cursor */
    pos savecurs;		       /* saved cursor position */
    int marg_t, marg_b;		       /* scroll margins */
    int dec_om;			       /* DEC origin mode flag */
    int wrap, wrapnext;		       /* wrap flags */
    int insert;			       /* insert-mode flag */
    int cset;			       /* 0 or 1: which char set */
    int save_cset, save_csattr;	       /* saved with cursor position */
    int save_utf, save_wnext;	       /* saved with cursor position */
    int rvideo;			       /* global reverse video flag */
    unsigned long rvbell_startpoint;   /* for ESC[?5hESC[?5l vbell */
    int cursor_on;		       /* cursor enabled flag */
    int reset_132;		       /* Flag ESC c resets to 80 cols */
    int use_bce;		       /* Use Background coloured erase */
    int blinker;		       /* When blinking is the cursor on ? */
    int tblinker;		       /* When the blinking text is on */
    int blink_is_real;		       /* Actually blink blinking text */
    int term_echoing;		       /* Does terminal want local echo? */
    int term_editing;		       /* Does terminal want local edit? */
    int sco_acs, save_sco_acs;	       /* CSI 10,11,12m -> OEM charset */
    int vt52_bold;		       /* Force bold on non-bold colours */
    int utf;			       /* Are we in toggleable UTF-8 mode? */
    int utf_state;		       /* Is there a pending UTF-8 character */
    int utf_char;		       /* and what is it so far. */
    int utf_size;		       /* The size of the UTF character. */
    int printing, only_printing;       /* Are we doing ANSI printing? */
    int print_state;		       /* state of print-end-sequence scan */
    bufchain printer_buf;	       /* buffered data for printer */
    printer_job *print_job;

    int rows, cols, savelines;
    int has_focus;
    int in_vbell;
    unsigned long vbell_startpoint;
    int app_cursor_keys, app_keypad_keys, vt52_mode;
    int repeat_off, cr_lf_return;
    int seen_disp_event;
    int big_cursor;

    long last_blink;		       /* used for real blinking control */
    long last_tblink;

    int xterm_mouse;		       /* send mouse messages to app */

    unsigned long cset_attr[2];

/*
 * Saved settings on the alternate screen.
 */
    int alt_x, alt_y, alt_om, alt_wrap, alt_wnext, alt_ins;
    int alt_cset, alt_sco_acs, alt_utf;
    int alt_t, alt_b;
    int alt_which;

#define ARGS_MAX 32		       /* max # of esc sequence arguments */
#define ARG_DEFAULT 0		       /* if an arg isn't specified */
#define def(a,d) ( (a) == ARG_DEFAULT ? (d) : (a) )
    int esc_args[ARGS_MAX];
    int esc_nargs;
    int esc_query;
#define ANSI(x,y)	((x)+((y)<<8))
#define ANSI_QUE(x)	ANSI(x,TRUE)

#define OSC_STR_MAX 2048
    int osc_strlen;
    char osc_string[OSC_STR_MAX + 1];
    int osc_w;

    char id_string[1024];

    unsigned char *tabs;

    enum {
	TOPLEVEL,
	SEEN_ESC,
	SEEN_CSI,
	SEEN_OSC,
	SEEN_OSC_W,

	DO_CTRLS,

	SEEN_OSC_P,
	OSC_STRING, OSC_MAYBE_ST,
	VT52_ESC,
	VT52_Y1,
	VT52_Y2,
	VT52_FG,
	VT52_BG
    } termstate;

    enum {
	NO_SELECTION, ABOUT_TO, DRAGGING, SELECTED
    } selstate;
    enum {
	LEXICOGRAPHIC, RECTANGULAR
    } seltype;
    enum {
	SM_CHAR, SM_WORD, SM_LINE
    } selmode;
    pos selstart, selend, selanchor;

    short wordness[256];

    wchar_t *paste_buffer;
    int paste_len, paste_pos, paste_hold;
    long last_paste;
};

#define in_utf(term) ((term)->utf || line_codepage==CP_UTF8)

#endif