/*  */
/* @(#)term.h	5.1 4/22/86 */

/*
 * term.h - this file is automatically made from caps and maketerm.ex.
 *
 * Guard against multiple includes.
 */

#ifndef auto_left_margin

#define auto_left_margin 		CUR Auto_left_margin
#define auto_right_margin 		CUR Auto_right_margin
#define beehive_glitch 			CUR Beehive_glitch
#define ceol_standout_glitch 		CUR Ceol_standout_glitch
#define eat_newline_glitch 		CUR Eat_newline_glitch
#define erase_overstrike 		CUR Erase_overstrike
#define generic_type 			CUR Generic_type
#define hard_copy 			CUR Hard_copy
#define has_meta_key 			CUR Has_meta_key
#define has_status_line 		CUR Has_status_line
#define insert_null_glitch 		CUR Insert_null_glitch
#define memory_above 			CUR Memory_above
#define memory_below 			CUR Memory_below
#define move_insert_mode 		CUR Move_insert_mode
#define move_standout_mode 		CUR Move_standout_mode
#define over_strike 			CUR Over_strike
#define status_line_esc_ok 		CUR Status_line_esc_ok
#define teleray_glitch 			CUR Teleray_glitch
#define tilde_glitch 			CUR Tilde_glitch
#define transparent_underline 		CUR Transparent_underline
#define xon_xoff 			CUR Xon_xoff
#define columns 			CUR Columns
#define init_tabs 			CUR Init_tabs
#define lines 				CUR Lines
#define lines_of_memory 		CUR Lines_of_memory
#define magic_cookie_glitch 		CUR Magic_cookie_glitch
#define padding_baud_rate 		CUR Padding_baud_rate
#define virtual_terminal 		CUR Virtual_terminal
#define width_status_line 		CUR Width_status_line
#define back_tab 			CUR strs.Back_tab
#define bell 				CUR strs.Bell
#define carriage_return 		CUR strs.Carriage_return
#define change_scroll_region 		CUR strs.Change_scroll_region
#define clear_all_tabs 			CUR strs.Clear_all_tabs
#define clear_screen 			CUR strs.Clear_screen
#define clr_eol 			CUR strs.Clr_eol
#define clr_eos 			CUR strs.Clr_eos
#define column_address 			CUR strs.Column_address
#define command_character 		CUR strs.Command_character
#define cursor_address 			CUR strs.Cursor_address
#define cursor_down 			CUR strs.Cursor_down
#define cursor_home 			CUR strs.Cursor_home
#define cursor_invisible 		CUR strs.Cursor_invisible
#define cursor_left 			CUR strs.Cursor_left
#define cursor_mem_address 		CUR strs.Cursor_mem_address
#define cursor_normal 			CUR strs.Cursor_normal
#define cursor_right 			CUR strs.Cursor_right
#define cursor_to_ll 			CUR strs.Cursor_to_ll
#define cursor_up 			CUR strs.Cursor_up
#define cursor_visible 			CUR strs.Cursor_visible
#define delete_character 		CUR strs.Delete_character
#define delete_line 			CUR strs.Delete_line
#define dis_status_line 		CUR strs.Dis_status_line
#define down_half_line 			CUR strs.Down_half_line
#define enter_alt_charset_mode 		CUR strs.Enter_alt_charset_mode
#define enter_blink_mode 		CUR strs.Enter_blink_mode
#define enter_bold_mode 		CUR strs.Enter_bold_mode
#define enter_ca_mode 			CUR strs.Enter_ca_mode
#define enter_delete_mode 		CUR strs.Enter_delete_mode
#define enter_dim_mode 			CUR strs.Enter_dim_mode
#define enter_insert_mode 		CUR strs.Enter_insert_mode
#define enter_secure_mode 		CUR strs.Enter_secure_mode
#define enter_protected_mode 		CUR strs.Enter_protected_mode
#define enter_reverse_mode 		CUR strs.Enter_reverse_mode
#define enter_standout_mode 		CUR strs.Enter_standout_mode
#define enter_underline_mode 		CUR strs.Enter_underline_mode
#define erase_chars 			CUR strs.Erase_chars
#define exit_alt_charset_mode 		CUR strs.Exit_alt_charset_mode
#define exit_attribute_mode 		CUR strs.Exit_attribute_mode
#define exit_ca_mode 			CUR strs.Exit_ca_mode
#define exit_delete_mode 		CUR strs.Exit_delete_mode
#define exit_insert_mode 		CUR strs.Exit_insert_mode
#define exit_standout_mode 		CUR strs.Exit_standout_mode
#define exit_underline_mode 		CUR strs.Exit_underline_mode
#define flash_screen 			CUR strs.Flash_screen
#define form_feed 			CUR strs.Form_feed
#define from_status_line 		CUR strs.From_status_line
#define init_1string 			CUR strs.Init_1string
#define init_2string 			CUR strs.Init_2string
#define init_3string 			CUR strs.Init_3string
#define init_file 			CUR strs.Init_file
#define insert_character 		CUR strs.Insert_character
#define insert_line 			CUR strs.Insert_line
#define insert_padding 			CUR strs.Insert_padding
#define key_backspace 			CUR strs.Key_backspace
#define key_catab 			CUR strs.Key_catab
#define key_clear 			CUR strs.Key_clear
#define key_ctab 			CUR strs.Key_ctab
#define key_dc 				CUR strs.Key_dc
#define key_dl 				CUR strs.Key_dl
#define key_down 			CUR strs.Key_down
#define key_eic 			CUR strs.Key_eic
#define key_eol 			CUR strs.Key_eol
#define key_eos 			CUR strs.Key_eos
#define key_f0 				CUR strs.Key_f0
#define key_f1 				CUR strs.Key_f1
#define key_f10 			CUR strs.Key_f10
#define key_f2 				CUR strs.Key_f2
#define key_f3 				CUR strs.Key_f3
#define key_f4 				CUR strs.Key_f4
#define key_f5 				CUR strs.Key_f5
#define key_f6 				CUR strs.Key_f6
#define key_f7 				CUR strs.Key_f7
#define key_f8 				CUR strs.Key_f8
#define key_f9 				CUR strs.Key_f9
#define key_home 			CUR strs.Key_home
#define key_ic 				CUR strs.Key_ic
#define key_il 				CUR strs.Key_il
#define key_left 			CUR strs.Key_left
#define key_ll 				CUR strs.Key_ll
#define key_npage 			CUR strs.Key_npage
#define key_ppage 			CUR strs.Key_ppage
#define key_right 			CUR strs.Key_right
#define key_sf 				CUR strs.Key_sf
#define key_sr 				CUR strs.Key_sr
#define key_stab 			CUR strs.Key_stab
#define key_up 				CUR strs.Key_up
#define keypad_local 			CUR strs.Keypad_local
#define keypad_xmit 			CUR strs.Keypad_xmit
#define lab_f0 				CUR strs.Lab_f0
#define lab_f1 				CUR strs.Lab_f1
#define lab_f10 			CUR strs.Lab_f10
#define lab_f2 				CUR strs.Lab_f2
#define lab_f3 				CUR strs.Lab_f3
#define lab_f4 				CUR strs.Lab_f4
#define lab_f5 				CUR strs.Lab_f5
#define lab_f6 				CUR strs.Lab_f6
#define lab_f7 				CUR strs.Lab_f7
#define lab_f8 				CUR strs.Lab_f8
#define lab_f9 				CUR strs2.Lab_f9
#define meta_off 			CUR strs2.Meta_off
#define meta_on 			CUR strs2.Meta_on
#define newline 			CUR strs2.Newline
#define pad_char 			CUR strs2.Pad_char
#define parm_dch 			CUR strs2.Parm_dch
#define parm_delete_line 		CUR strs2.Parm_delete_line
#define parm_down_cursor 		CUR strs2.Parm_down_cursor
#define parm_ich 			CUR strs2.Parm_ich
#define parm_index 			CUR strs2.Parm_index
#define parm_insert_line 		CUR strs2.Parm_insert_line
#define parm_left_cursor 		CUR strs2.Parm_left_cursor
#define parm_right_cursor 		CUR strs2.Parm_right_cursor
#define parm_rindex 			CUR strs2.Parm_rindex
#define parm_up_cursor 			CUR strs2.Parm_up_cursor
#define pkey_key 			CUR strs2.Pkey_key
#define pkey_local 			CUR strs2.Pkey_local
#define pkey_xmit 			CUR strs2.Pkey_xmit
#define print_screen 			CUR strs2.Print_screen
#define prtr_off 			CUR strs2.Prtr_off
#define prtr_on 			CUR strs2.Prtr_on
#define repeat_char 			CUR strs2.Repeat_char
#define reset_1string 			CUR strs2.Reset_1string
#define reset_2string 			CUR strs2.Reset_2string
#define reset_3string 			CUR strs2.Reset_3string
#define reset_file 			CUR strs2.Reset_file
#define restore_cursor 			CUR strs2.Restore_cursor
#define row_address 			CUR strs2.Row_address
#define save_cursor 			CUR strs2.Save_cursor
#define scroll_forward 			CUR strs2.Scroll_forward
#define scroll_reverse 			CUR strs2.Scroll_reverse
#define set_attributes 			CUR strs2.Set_attributes
#define set_tab 			CUR strs2.Set_tab
#define set_window 			CUR strs2.Set_window
#define tab 				CUR strs2.Tab
#define to_status_line 			CUR strs2.To_status_line
#define underline_char 			CUR strs2.Underline_char
#define up_half_line 			CUR strs2.Up_half_line
#define init_prog 			CUR strs2.Init_prog
#define key_a1 				CUR strs2.Key_a1
#define key_a3 				CUR strs2.Key_a3
#define key_b2 				CUR strs2.Key_b2
#define key_c1 				CUR strs2.Key_c1
#define key_c3 				CUR strs2.Key_c3
#define prtr_non 			CUR strs2.Prtr_non
typedef char *charptr;

struct strs {
    charptr
	Back_tab,		/* Back tab (P) */
	Bell,			/* Audible signal (bell) (P) */
	Carriage_return,	/* Carriage return (P*) */
	Change_scroll_region,	/* change to lines #1 thru #2 (vt100) (PG) */
	Clear_all_tabs,		/* Clear all tab stops. (P) */
	Clear_screen,		/* Clear screen (P*) */
	Clr_eol,		/* Clear to end of line (P) */
	Clr_eos,		/* Clear to end of display (P*) */
	Column_address,		/* Set cursor column (PG) */
	Command_character,	/* Term. settable cmd char in prototype */
	Cursor_address,		/* Cursor motion to row #1 col #2 (PG) */
	Cursor_down,		/* Down one line */
	Cursor_home,		/* Home cursor (if no cup) */
	Cursor_invisible,	/* Make cursor invisible */
	Cursor_left,		/* Move cursor left one space. */
	Cursor_mem_address,	/* Memory relative cursor addressing. */
	Cursor_normal,		/* Make cursor appear normal (undo vs/vi) */
	Cursor_right,		/* Non-destructive space (cursor right) */
	Cursor_to_ll,		/* Last line, first column (if no cup) */
	Cursor_up,		/* Upline (cursor up) */
	Cursor_visible,		/* Make cursor very visible */
	Delete_character,	/* Delete character (P*) */
	Delete_line,		/* Delete line (P*) */
	Dis_status_line,	/* Disable status line */
	Down_half_line,		/* Half-line down (forward 1/2 linefeed) */
	Enter_alt_charset_mode,	/* Start alternate character set (P) */
	Enter_blink_mode,	/* Turn on blinking */
	Enter_bold_mode,	/* Turn on bold (extra bright) mode */
	Enter_ca_mode,		/* String to begin programs that use cup */
	Enter_delete_mode,	/* Delete mode (enter) */
	Enter_dim_mode,		/* Turn on half-bright mode */
	Enter_insert_mode,	/* Insert mode (enter); */
	Enter_secure_mode,	/* Turn on blank mode (chars invisible) */
	Enter_protected_mode,	/* Turn on protected mode */
	Enter_reverse_mode,	/* Turn on reverse video mode */
	Enter_standout_mode,	/* Begin stand out mode */
	Enter_underline_mode,	/* Start underscore mode */
	Erase_chars,		/* Erase #1 characters (PG) */
	Exit_alt_charset_mode,	/* End alternate character set (P) */
	Exit_attribute_mode,	/* Turn off all attributes */
	Exit_ca_mode,		/* String to end programs that use cup */
	Exit_delete_mode,	/* End delete mode */
	Exit_insert_mode,	/* End insert mode; */
	Exit_standout_mode,	/* End stand out mode */
	Exit_underline_mode,	/* End underscore mode */
	Flash_screen,		/* Visible bell (may not move cursor) */
	Form_feed,		/* Hardcopy terminal page eject (P*) */
	From_status_line,	/* Return from status line */
	Init_1string,		/* Terminal initialization string */
	Init_2string,		/* Terminal initialization string */
	Init_3string,		/* Terminal initialization string */
	Init_file,		/* Name of file containing is */
	Insert_character,	/* Insert character (P) */
	Insert_line,		/* Add new blank line (P*) */
	Insert_padding,		/* Insert pad after character inserted (P*) */
	Key_backspace,		/* Sent by backspace key */
	Key_catab,		/* Sent by clear-all-tabs key. */
	Key_clear,		/* Sent by clear screen or erase key. */
	Key_ctab,		/* Sent by clear-tab key */
	Key_dc,			/* Sent by delete character key. */
	Key_dl,			/* Sent by delete line key. */
	Key_down,		/* Sent by terminal down arrow key */
	Key_eic,		/* Sent by rmir or smir in insert mode. */
	Key_eol,		/* Sent by clear-to-end-of-line key. */
	Key_eos,		/* Sent by clear-to-end-of-screen key. */
	Key_f0,			/* Sent by function key f0. */
	Key_f1,			/* Sent by function key f1. */
	Key_f10,		/* Sent by function key f10. */
	Key_f2,			/* Sent by function key f2. */
	Key_f3,			/* Sent by function key f3. */
	Key_f4,			/* Sent by function key f4. */
	Key_f5,			/* Sent by function key f5. */
	Key_f6,			/* Sent by function key f6. */
	Key_f7,			/* Sent by function key f7. */
	Key_f8,			/* Sent by function key f8. */
	Key_f9,			/* Sent by function key f9. */
	Key_home,		/* Sent by home key. */
	Key_ic,			/* Sent by ins char/enter ins mode key. */
	Key_il,			/* Sent by insert line. */
	Key_left,		/* Sent by terminal left arrow key */
	Key_ll,			/* Sent by home-down key */
	Key_npage,		/* Sent by next-page key */
	Key_ppage,		/* Sent by previous-page key */
	Key_right,		/* Sent by terminal right arrow key */
	Key_sf,			/* Sent by scroll-forward/down key */
	Key_sr,			/* Sent by scroll-backward/up key */
	Key_stab,		/* Sent by set-tab key */
	Key_up,			/* Sent by terminal up arrow key */
	Keypad_local,		/* Out of "keypad transmit" mode */
	Keypad_xmit,		/* Put terminal in "keypad transmit" mode */
	Lab_f0,			/* Labels on function key f0 if not f0 */
	Lab_f1,			/* Labels on function key f1 if not f1 */
	Lab_f10,		/* Labels on function key f10 if not f10 */
	Lab_f2,			/* Labels on function key f2 if not f2 */
	Lab_f3,			/* Labels on function key f3 if not f3 */
	Lab_f4,			/* Labels on function key f4 if not f4 */
	Lab_f5,			/* Labels on function key f5 if not f5 */
	Lab_f6,			/* Labels on function key f6 if not f6 */
	Lab_f7,			/* Labels on function key f7 if not f7 */
	Lab_f8;			/* Labels on function key f8 if not f8 */
};
struct strs2 {
    charptr
	Lab_f9,			/* Labels on function key f9 if not f9 */
	Meta_off,		/* Turn off "meta mode" */
	Meta_on,		/* Turn on "meta mode" (8th bit) */
	Newline,		/* Newline (behaves like cr followed by lf) */
	Pad_char,		/* Pad character (rather than null) */
	Parm_dch,		/* Delete #1 chars (PG*) */
	Parm_delete_line,	/* Delete #1 lines (PG*) */
	Parm_down_cursor,	/* Move cursor down #1 lines. (PG*) */
	Parm_ich,		/* Insert #1 blank chars (PG*) */
	Parm_index,		/* Scroll forward #1 lines. (PG) */
	Parm_insert_line,	/* Add #1 new blank lines (PG*) */
	Parm_left_cursor,	/* Move cursor left #1 spaces (PG) */
	Parm_right_cursor,	/* Move cursor right #1 spaces. (PG*) */
	Parm_rindex,		/* Scroll backward #1 lines. (PG) */
	Parm_up_cursor,		/* Move cursor up #1 lines. (PG*) */
	Pkey_key,		/* Prog funct key #1 to type string #2 */
	Pkey_local,		/* Prog funct key #1 to execute string #2 */
	Pkey_xmit,		/* Prog funct key #1 to xmit string #2 */
	Print_screen,		/* Print contents of the screen */
	Prtr_off,		/* Turn off the printer */
	Prtr_on,		/* Turn on the printer */
	Repeat_char,		/* Repeat char #1 #2 times.  (PG*) */
	Reset_1string,		/* Reset terminal completely to sane modes. */
	Reset_2string,		/* Reset terminal completely to sane modes. */
	Reset_3string,		/* Reset terminal completely to sane modes. */
	Reset_file,		/* Name of file containing reset string. */
	Restore_cursor,		/* Restore cursor to position of last sc. */
	Row_address,		/* Like hpa but sets row. (PG) */
	Save_cursor,		/* Save cursor position. (P) */
	Scroll_forward,		/* Scroll text up (P) */
	Scroll_reverse,		/* Scroll text down (P) */
	Set_attributes,		/* Define the video attributes (PG9) */
	Set_tab,		/* Set a tab in all rows, current column. */
	Set_window,		/* Current window is lines #1-#2 cols #3-#4 */
	Tab,			/* Tab to next 8 space hardware tab stop. */
	To_status_line,		/* Go to status line */
	Underline_char,		/* Underscore one char and move past it */
	Up_half_line,		/* Half-line up (reverse 1/2 linefeed) */
	Init_prog,		/* Path name of program for init. */
	Key_a1,			/* Upper left of keypad */
	Key_a3,			/* Upper right of keypad */
	Key_b2,			/* Center of keypad */
	Key_c1,			/* Lower left of keypad */
	Key_c3,			/* Lower right of keypad */
	Prtr_non;		/* Turn on the printer for #1 bytes. */
};

struct term {
    char
	Auto_left_margin,	/* cub1 wraps from column 0 to last column */
	Auto_right_margin,	/* Terminal has automatic margins */
	Beehive_glitch,		/* Beehive (f1=escape, f2=ctrl C) */
	Ceol_standout_glitch,	/* Standout not erased by overwriting (hp) */
	Eat_newline_glitch,	/* newline ignored after 80 cols (Concept) */
	Erase_overstrike,	/* Can erase overstrikes with a blank */
	Generic_type,		/* Generic line type (e.g. dialup, switch). */
	Hard_copy,		/* Hardcopy terminal */
	Has_meta_key,		/* Has a meta key (shift, sets parity bit) */
	Has_status_line,	/* Has extra "status line" */
	Insert_null_glitch,	/* Insert mode distinguishes nulls */
	Memory_above,		/* Display may be retained above the screen */
	Memory_below,		/* Display may be retained below the screen */
	Move_insert_mode,	/* Safe to move while in insert mode */
	Move_standout_mode,	/* Safe to move in standout modes */
	Over_strike,		/* Terminal overstrikes */
	Status_line_esc_ok,	/* Escape can be used on the status line */
	Teleray_glitch,		/* Tabs destructive, magic so char (t1061) */
	Tilde_glitch,		/* Hazeltine; can't print ~'s */
	Transparent_underline,	/* underline character overstrikes */
	Xon_xoff;		/* Terminal uses xon/xoff handshaking */
    short
	Columns,		/* Number of columns in a line */
	Init_tabs,		/* Tabs initially every # spaces. */
	Lines,			/* Number of lines on screen or page */
	Lines_of_memory,	/* Lines of memory if > lines.  0 => varies */
	Magic_cookie_glitch,	/* Number blank chars left by smso or rmso */
	Padding_baud_rate,	/* Lowest baud rate where padding needed */
	Virtual_terminal,	/* Virtual terminal number (CB/Unix) */
	Width_status_line;	/* # columns in status line */
	struct strs strs;
	struct strs2 strs2;
	short Filedes;		/* file descriptor being written to */
#ifndef NONSTANDARD
	SGTTY Ottyb,		/* original state of the terminal */
	      Nttyb;		/* current state of the terminal */
#endif
};
#ifndef NONSTANDARD
extern struct term *cur_term;
#endif

#endif auto_left_margin
 
#ifdef SINGLE
extern struct term _first_term;
# define CUR	_first_term.
#else
# define CUR	cur_term->
#endif
