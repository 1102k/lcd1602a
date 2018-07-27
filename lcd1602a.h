#ifndef __LIB1602A_H__
#define __LIB1602A_H__

#ifndef  NULL 
#define  NULL            (void*)0
#endif

typedef  enum 
{
LIB1602A_FALSE,
LIB1602A_TRUE
}lib1602a_bool_t;


typedef struct
{
 void (*io_e_set)(void);
 void (*io_e_clr)(void);
 void (*io_rs_set)(void);
 void (*io_rs_clr)(void);
 void (*io_rw_set)(void);
 void (*io_rw_clr)(void);
 uint8_t (*io_data_in)(void);
 void (*io_data_out)(uint8_t);
 void (*io_delay_ms)(uint8_t);
}lib1602a_io_driver_t;


typedef enum
{
 LIB1602A_STATUS_OK =0,
 LIB1602A_STATUS_ERR,
 LIB1602A_STATUS_RDY,
 LIB1602_STATUS_BUSY,
 LIB1602_STATUS_WR_TIMEOUT,
 LIB1602_STATUS_RD_TIMEOUT
}lib1602a_status_t;


typedef enum
{
LIB1602A_POS_LINE_1,
LIB1602A_POS_LINE_2	
}lib1602a_pos_line_t;

typedef enum
{
LIB1602A_ADDR_DIR_INCREASE,
LIB1602A_ADDR_DIR_DECREASE
}lib1602a_addr_dir_t;

typedef enum
{
LIB1602A_SCREEN_DISPLAY_ON,
LIB1602A_SCREEN_DISPLAY_OFF
}lib1602a_screen_display_t;



typedef enum
{
LIB1602A_CURSOR_DISPLAY_ON,
LIB1602A_CURSOR_DISPLAY_OFF
}lib1602a_cursor_display_t;


typedef enum
{
LIB1602A_CURSOR_TYPE_NO_BLINK,
LIB1602A_CURSOR_TYPE_BLINK
}lib1602a_cursor_type_t;

typedef struct
{
lib1602a_addr_dir_t        addr_dir;
lib1602a_bool_t            shift_screen;
lib1602a_screen_display_t  screen_display;
lib1602a_cursor_display_t  cursor_display;
lib1602a_cursor_type_t     cursor_type;
}lib1602a_config_t;




lib1602a_status_t lib1602a_register_io_driver(lib1602a_io_driver_t *ptr_io_drv);
lib1602a_status_t lib1602a_config(lib1602a_config_t *ptr_config);
lib1602a_status_t lib1602a_screen_cursor_ctrl(lib1602a_screen_display_t screen,lib1602a_cursor_display_t cursor,lib1602a_cursor_type_t cursor_type);

lib1602a_status_t lib1602a_scroll_screen_left(void);
lib1602a_status_t lib1602a_scroll_screen_right(void);
lib1602a_status_t lib1602a_clear_screen(void);

lib1602a_status_t lib1602a_move_cursor_left(void);
lib1602a_status_t lib1602a_move_cursor_right(void);

lib1602a_status_t lib1602a_set_cursor_pos(lib1602a_pos_line_t line ,uint8_t x);
lib1602a_status_t lib1602a_get_cursor_pos(uint8_t *ptr_pos);

lib1602a_status_t lib1602a_display_str(const char *ptr_str,lib1602a_pos_line_t line ,uint8_t x);
lib1602a_status_t lib1602a_int_to_str_hex(char **ptr_str,int32_t num ,uint8_t width,lib1602a_bool_t prifix);
lib1602a_status_t lib1602a_int_to_str_dec(char **ptr_str,int32_t num ,uint8_t width); 














#endif









