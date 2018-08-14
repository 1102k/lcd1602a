#include "stdint.h"
#include "lcd1602a.h"

               
/*1602A  字符(共80个)  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  ...不可见 */
/*                   --------------------------------------------------------           */
/*1602A LINE1 地址  |0x00 ...............................................0x0F|...0x27   */
/*1602A LINE2 地址  |0x40 ...............................................0x4F|...0x67   */
/*                   --------------------------------------------------------           */


static lib1602a_io_driver_t *driver;

/*clear screen icode*/
#define  LIB1602A_CLEAR_SCREEN_I_CODE                  (1<<0)

/*return home icode*/
#define  LIB1602A_RETURN_HOME_I_CODE                   (1<<1)


/*entry mode set icode*/
#define  LIB1602A_ADDR_CTRL_I_CODE                     (1<<2)
#define  LIB1602A_ADDR_DIR_CTRL_BIT_POS                 1  
#define  LIB1602A_ADDR_SHIFT_SCREEN_BIT_POS             0

#define  LIB1602A_ADDR_DIR_INCREASE                     1 
#define  LIB1602A_ADDR_DIR_DECREASE                     0   

#define  LIB1602A_ADDR_SHIFT_SCREEN                     1
#define  LIB1602A_ADDR_NO_SHIFT_SCREEN                  0

/*display on off crl icode*/
#define  LIB1602A_DISPLAY_ON_OFF_CTRL_I_CODE           (1<<3)
#define  LIB1602A_SCREEN_ON_OFF_CTRL_BIT_POS            2   
#define  LIB1602A_CURSOR_ON_OFF_CTRL_BIT_POS            1 
#define  LIB1602A_CURSOR_BLINK_CTRL_BIT_POS             0   

#define  LIB1602A_SCREEN_ON                             1
#define  LIB1602A_SCREEN_OFF                            0

#define  LIB1602A_CURSOR_ON                             1
#define  LIB1602A_CURSOR_OFF                            0

#define  LIB1602A_CURSOR_BLINK                          1
#define  LIB1602A_CURSOR_NO_BLINK                       0

/*cursor or display shift*/
#define  LIB1602A_CS_CTRL_I_CODE                       (1<<4)
#define  LIB1602A_CS_SELECT_CTRL_BIT_POS                3   
#define  LIB1602A_CS_DIR_CTRL_BIT_POS                   2 

#define  LIB1602A_CS_SELECT_CURSOR                      0  
#define  LIB1602A_CS_SELECT_SCREEN_SHIFT                1   


#define  LIB1602A_CS_DIR_LEFT                           0
#define  LIB1602A_CS_DIR_RIGHT                          1


/*fucntion set*/
#define  LIB1602A_FUNCTION_CTRL_I_CODE                 (1<<5)
#define  LIB1602A_FUNCTION_DATA_CTRL_BIT_POS            4   
#define  LIB1602A_FUNCTION_LINE_CTRL_BIT_POS            3 
#define  LIB1602A_FUNCTION_FONT_CTRL_BIT_POS            2

#define  LIB1602A_FUNCTION_DATA8                        1 
#define  LIB1602A_FUNCTION_DATA4                        0 

#define  LIB1602A_FUNCTION_LINE_2                       1
#define  LIB1602A_FUNCTION_LINE_1                       0   


#define  LIB1602A_FUNCTION_FONT_5X8                     0
#define  LIB1602A_FUNCTION_FONT_5X11                    1


/*set cgram address*/
#define  LIB1602A_CGRAM_ADDR_CTRL_I_CODE               (1<<6)
#define  LIB1602A_CGRAM_ADDR_CTRL_MASK                 ((1<<6)-1)

/*set ddram address*/
#define  LIB1602A_DDRAM_ADDR_CTRL_I_CODE               (1<<7)
#define  LIB1602A_DDRAM_ADDR_CTRL_MASK                 ((1<<7)-1)





/*chip enbale*/
#define  LIB1602A_CHIP_ENABLE()                        driver->io_e_clr()
#define  LIB1602A_CHIP_DISABLE()                       driver->io_e_set()

/*read or write enable*/

#define  LIB1602A_WRITE_ENABLE()                       driver->io_rw_clr()
#define  LIB1602A_READ_ENABLE()                        driver->io_rw_set()


/*reg or ram */

#define  LIB1602A_REG_ENABLE()                         driver->io_rs_clr()
#define  LIB1602A_RAM_ENABLE()                         driver->io_rs_set()


/*busy flag mask*/
#define  LIB1602A_BUSY_MASK                            (1<<7)



lib1602a_status_t lib1602a_register_io_driver(lib1602a_io_driver_t *ptr_io_drv)
{
if(ptr_io_drv == NULL)
 return LIB1602A_STATUS_ERR;
 
driver=ptr_io_drv;

return LIB1602A_STATUS_OK;
}

lib1602a_bool_t lib1602a_is_busy(void)
{
 lib1602a_bool_t status=LIB1602A_FALSE;
 uint8_t busy;
 
 driver->io_data_out(0xff);
 LIB1602A_REG_ENABLE();
 LIB1602A_READ_ENABLE();
 
 driver->io_delay_ms(1);
 LIB1602A_CHIP_DISABLE();
 driver->io_delay_ms(1);
 
 busy=driver->io_data_in();
 LIB1602A_CHIP_ENABLE();
 
 if(busy & LIB1602A_BUSY_MASK)/*is busy*/
 status = LIB1602A_TRUE;
 
 return status;
}

lib1602a_status_t lib1602a_get_cursor_pos(uint8_t *ptr_pos)
{
 uint8_t status;
 
 driver->io_data_out(0xff);
 LIB1602A_REG_ENABLE();
 LIB1602A_READ_ENABLE();
 
 driver->io_delay_ms(1);
 LIB1602A_CHIP_DISABLE();   
 driver->io_delay_ms(1);
 
 status=driver->io_data_in();
 LIB1602A_CHIP_ENABLE();
 
 *ptr_pos=status & LIB1602A_DDRAM_ADDR_CTRL_MASK;/*addr*/
 
 return LIB1602A_STATUS_OK;
}


static lib1602a_bool_t lib1602a_is_driver_valid(void)
{
return ((driver == NULL)?LIB1602A_FALSE:LIB1602A_TRUE);
}

static lib1602a_status_t lib1602a_entry_mode(lib1602a_addr_dir_t addr_dir,lib1602a_bool_t shift_screen)
{
 uint8_t icode;

 if(lib1602a_is_driver_valid() == LIB1602A_FALSE)
 return LIB1602A_STATUS_ERR;
 
 LIB1602A_REG_ENABLE();
 LIB1602A_WRITE_ENABLE();
 LIB1602A_CHIP_DISABLE();
 driver->io_delay_ms(1);
 icode=0;

 switch(addr_dir)
 {
 case LIB1602A_ADDR_DIR_INCREASE:
	 icode|=(LIB1602A_ADDR_DIR_INCREASE<<LIB1602A_ADDR_DIR_CTRL_BIT_POS);
	 break;
 case LIB1602A_ADDR_DIR_DECREASE:
	 icode|=(LIB1602A_ADDR_DIR_DECREASE<<LIB1602A_ADDR_DIR_CTRL_BIT_POS);
	 break;
 }

 if (shift_screen)
 {
 icode|=(LIB1602A_ADDR_SHIFT_SCREEN<<LIB1602A_ADDR_SHIFT_SCREEN_BIT_POS);
 }
 else
 {
 icode|=(LIB1602A_ADDR_NO_SHIFT_SCREEN<<LIB1602A_ADDR_SHIFT_SCREEN_BIT_POS);
 }

 driver->io_data_out(icode|LIB1602A_ADDR_CTRL_I_CODE);
 driver->io_delay_ms(1);
 LIB1602A_CHIP_ENABLE();
 driver->io_delay_ms(1);
 
 return LIB1602A_STATUS_OK;
}

static lib1602a_status_t lib1602a_set_default_function(void)
{
 uint8_t icode;

 if(lib1602a_is_driver_valid() == LIB1602A_FALSE)
 return LIB1602A_STATUS_ERR;
 

 LIB1602A_REG_ENABLE();
 LIB1602A_WRITE_ENABLE();
 LIB1602A_CHIP_DISABLE();
 driver->io_delay_ms(1);
 
 icode=0;
 icode|=(LIB1602A_FUNCTION_DATA8<<LIB1602A_FUNCTION_DATA_CTRL_BIT_POS)|(LIB1602A_FUNCTION_LINE_2<<LIB1602A_FUNCTION_LINE_CTRL_BIT_POS)|(LIB1602A_FUNCTION_FONT_5X8<<LIB1602A_FUNCTION_FONT_CTRL_BIT_POS);
 driver->io_data_out(icode|LIB1602A_FUNCTION_CTRL_I_CODE);

 driver->io_delay_ms(1);
 LIB1602A_CHIP_ENABLE();
 driver->io_delay_ms(1);
 return LIB1602A_STATUS_OK;
}

lib1602a_status_t lib1602a_screen_cursor_ctrl(lib1602a_screen_display_t screen,lib1602a_cursor_display_t cursor,lib1602a_cursor_type_t cursor_type)
{
uint8_t icode;

if(lib1602a_is_driver_valid() == LIB1602A_FALSE)
return  LIB1602A_STATUS_ERR;


LIB1602A_WRITE_ENABLE();
LIB1602A_REG_ENABLE();
LIB1602A_CHIP_DISABLE();
driver->io_delay_ms(1);
/*display cursor*/
icode=0;
if(screen == LIB1602A_SCREEN_DISPLAY_ON)
icode|=(LIB1602A_SCREEN_ON<<LIB1602A_SCREEN_ON_OFF_CTRL_BIT_POS);
else 
icode|=(LIB1602A_SCREEN_OFF<<LIB1602A_SCREEN_ON_OFF_CTRL_BIT_POS);

if(cursor == LIB1602A_CURSOR_DISPLAY_ON)
icode|=(LIB1602A_CURSOR_ON<<LIB1602A_CURSOR_ON_OFF_CTRL_BIT_POS);
else 
icode|=(LIB1602A_CURSOR_OFF<<LIB1602A_CURSOR_ON_OFF_CTRL_BIT_POS);

if(cursor_type == LIB1602A_CURSOR_TYPE_BLINK)
icode|=(LIB1602A_CURSOR_BLINK<<LIB1602A_CURSOR_BLINK_CTRL_BIT_POS);
else 
icode|=(LIB1602A_CURSOR_NO_BLINK<<LIB1602A_CURSOR_BLINK_CTRL_BIT_POS);


driver->io_data_out(icode|LIB1602A_DISPLAY_ON_OFF_CTRL_I_CODE);
driver->io_delay_ms(1);
LIB1602A_CHIP_ENABLE();
driver->io_delay_ms(1);

return LIB1602A_STATUS_OK;
}

lib1602a_status_t lib1602a_config(lib1602a_config_t *ptr_config)
{
if(driver == NULL)
 return LIB1602A_STATUS_ERR;
if(ptr_config == NULL)
 return LIB1602A_STATUS_ERR;

driver->io_delay_ms(5);
lib1602a_set_default_function();
lib1602a_entry_mode(ptr_config->addr_dir,ptr_config->shift_screen);
lib1602a_screen_cursor_ctrl(ptr_config->screen_display,ptr_config->cursor_display,ptr_config->cursor_type);

return LIB1602A_STATUS_OK;
}



lib1602a_status_t lib1602a_scroll_screen_left(void)
{
 uint8_t icode;
 
 if(lib1602a_is_driver_valid() == LIB1602A_FALSE)
 return  LIB1602A_STATUS_ERR;
 
 LIB1602A_WRITE_ENABLE();
 LIB1602A_REG_ENABLE();
 LIB1602A_CHIP_DISABLE();
 driver->io_delay_ms(1);
 
 icode=0;
 icode|=(LIB1602A_CS_SELECT_SCREEN_SHIFT<<LIB1602A_CS_SELECT_CTRL_BIT_POS)|(LIB1602A_CS_DIR_LEFT<<LIB1602A_CS_DIR_CTRL_BIT_POS);
 
 driver->io_data_out(icode|LIB1602A_CS_CTRL_I_CODE);
 driver->io_delay_ms(1);
 LIB1602A_CHIP_ENABLE();
 driver->io_delay_ms(1);

 return LIB1602A_STATUS_OK;
}


lib1602a_status_t lib1602a_scroll_screen_right(void)
{
 uint8_t icode;	
 
 if(lib1602a_is_driver_valid() == LIB1602A_FALSE)
 return	LIB1602A_STATUS_ERR;
	
 LIB1602A_WRITE_ENABLE();
 LIB1602A_REG_ENABLE();
 LIB1602A_CHIP_DISABLE();
 driver->io_delay_ms(1);
 
 icode=0;
 icode|=(LIB1602A_CS_SELECT_SCREEN_SHIFT<<LIB1602A_CS_SELECT_CTRL_BIT_POS)|(LIB1602A_CS_DIR_RIGHT<<LIB1602A_CS_DIR_CTRL_BIT_POS);
	
 driver->io_data_out(icode|LIB1602A_CS_CTRL_I_CODE);
 driver->io_delay_ms(1);
 LIB1602A_CHIP_ENABLE();
 driver->io_delay_ms(1);
	
 return LIB1602A_STATUS_OK;
}


lib1602a_status_t lib1602a_clear_screen(void)

{
	uint8_t icode; 
	
	if(lib1602a_is_driver_valid() == LIB1602A_FALSE)
	return LIB1602A_STATUS_ERR;
	   
	LIB1602A_WRITE_ENABLE();
	LIB1602A_REG_ENABLE();
    LIB1602A_CHIP_DISABLE();
	driver->io_delay_ms(1);   
	icode=0;   
	driver->io_data_out(icode|LIB1602A_CLEAR_SCREEN_I_CODE);
    driver->io_delay_ms(1);
	LIB1602A_CHIP_ENABLE();
	driver->io_delay_ms(3);
	   
	return LIB1602A_STATUS_OK;

}

lib1602a_status_t lib1602a_move_cursor_left(void)
{
	uint8_t icode; 
	
	if(lib1602a_is_driver_valid() == LIB1602A_FALSE)
	return LIB1602A_STATUS_ERR;
	   
	LIB1602A_WRITE_ENABLE();
	LIB1602A_REG_ENABLE();
	LIB1602A_CHIP_DISABLE();
    driver->io_delay_ms(1);
    
	icode=0;
	icode|=(LIB1602A_CS_SELECT_CURSOR<<LIB1602A_CS_SELECT_CTRL_BIT_POS)|(LIB1602A_CS_DIR_LEFT<<LIB1602A_CS_DIR_CTRL_BIT_POS);
	   
	driver->io_data_out(icode|LIB1602A_CS_CTRL_I_CODE);
    driver->io_delay_ms(1);
	LIB1602A_CHIP_ENABLE();
	driver->io_delay_ms(1);
	   
	return LIB1602A_STATUS_OK;

}
lib1602a_status_t lib1602a_move_cursor_right(void)
{
	uint8_t icode; 
	
	if(lib1602a_is_driver_valid() == LIB1602A_FALSE)
	return LIB1602A_STATUS_ERR;
	   
	LIB1602A_WRITE_ENABLE();
	LIB1602A_REG_ENABLE();
    LIB1602A_CHIP_DISABLE();
    driver->io_delay_ms(1);   
	icode=0;
	icode|=(LIB1602A_CS_SELECT_CURSOR<<LIB1602A_CS_SELECT_CTRL_BIT_POS)|(LIB1602A_CS_DIR_RIGHT<<LIB1602A_CS_DIR_CTRL_BIT_POS);
	   
	driver->io_data_out(icode|LIB1602A_CS_CTRL_I_CODE);
    driver->io_delay_ms(1);
	LIB1602A_CHIP_ENABLE();
	driver->io_delay_ms(1);
	   
	return LIB1602A_STATUS_OK;

}
lib1602a_status_t lib1602a_move_cursor_up(void);
lib1602a_status_t lib1602a_move_cursor_down(void);


lib1602a_status_t lib1602a_set_cursor_pos(lib1602a_pos_line_t line ,uint8_t x)
{
	uint8_t icode; 
	uint8_t ddram_addr;
	if(lib1602a_is_driver_valid() == LIB1602A_FALSE)
	return LIB1602A_STATUS_ERR;
	
	LIB1602A_WRITE_ENABLE();
	LIB1602A_REG_ENABLE();
    LIB1602A_CHIP_DISABLE();
    driver->io_delay_ms(1);
    
    if(line == LIB1602A_POS_LINE_1)
	ddram_addr=x;
	else
    ddram_addr=0x40+x;
	
	icode=0;
	icode|=(ddram_addr & LIB1602A_DDRAM_ADDR_CTRL_MASK);
	driver->io_data_out(icode|LIB1602A_DDRAM_ADDR_CTRL_I_CODE);
    driver->io_delay_ms(1);
	LIB1602A_CHIP_ENABLE();
	driver->io_delay_ms(1);
	   
	return LIB1602A_STATUS_OK;
}

lib1602a_status_t lib1602a_display_str(const char *ptr_str,lib1602a_pos_line_t line ,uint8_t x)
{
	uint8_t ddram_code;
	if(lib1602a_is_driver_valid() == LIB1602A_FALSE)
	return LIB1602A_STATUS_ERR;
    
    if(ptr_str == NULL)
    return LIB1602A_STATUS_ERR;
    
	lib1602a_set_cursor_pos(line,x);

	LIB1602A_WRITE_ENABLE();
	LIB1602A_RAM_ENABLE();
    driver->io_delay_ms(1);
	while(*ptr_str!='\0')
	{
    LIB1602A_CHIP_DISABLE();
	ddram_code=*ptr_str;
	driver->io_data_out(ddram_code);
	LIB1602A_CHIP_ENABLE();
	driver->io_delay_ms(1);
	ptr_str++;
	}
    
   return LIB1602A_STATUS_OK;  	
}


#define  LIB1602A_HEX_STR_WIDTH_MAX            8
#define  LIB1602A_DEC_STR_WIDTH_MAX            8

static char hex_str[LIB1602A_HEX_STR_WIDTH_MAX+3];
static char dec_str[LIB1602A_DEC_STR_WIDTH_MAX+2];

lib1602a_status_t lib1602a_int_to_str_hex(char **ptr_str,int32_t num ,uint8_t width,lib1602a_bool_t prifix )
{
  char *ptr_temp;
  uint8_t num_temp;
  
  if(ptr_str == NULL)
  return LIB1602A_STATUS_ERR;
  if(width < 1 || width >LIB1602A_HEX_STR_WIDTH_MAX)
    width=LIB1602A_HEX_STR_WIDTH_MAX;
   
 ptr_temp=hex_str+width+2;
 *ptr_temp='\0';
 ptr_temp--;
 while(width--)
 {
  num_temp=num & 0x0f;
  if(num_temp > 9)
  *ptr_temp= 'A'+(num_temp-10);
  else
  *ptr_temp= '0'+num_temp; 
  
  num>>=4;
  ptr_temp--;
 }
 if(prifix == LIB1602A_TRUE)
 {
  hex_str[0]='0';
  hex_str[1]='x';
  *ptr_str=hex_str;
 }
 else
 {
 *ptr_str=hex_str+2;  
 }
  
  return LIB1602A_STATUS_OK;
}

lib1602a_status_t lib1602a_int_to_str_dec(char **ptr_str,int32_t num ,uint8_t width )
{
  char *ptr_temp;
  uint8_t num_temp;
  
  if(ptr_str == NULL)
  return LIB1602A_STATUS_ERR;
  if(width < 1 || width >LIB1602A_DEC_STR_WIDTH_MAX)
    width=LIB1602A_DEC_STR_WIDTH_MAX;
  
  if(num < 0)
  {
    num*=-1;
    ptr_temp=dec_str+width+1;
    dec_str[0]='-';
  }
  else
  {
   ptr_temp=dec_str+width; 
  }
  
 *ptr_temp='\0';
 ptr_temp--;
 while(width--)
 {
  num_temp=num % 10;
  *ptr_temp= '0'+num_temp; 
  num/=10;
  ptr_temp--;
 }
 
  *ptr_str=dec_str;
  
 return LIB1602A_STATUS_OK;
}














