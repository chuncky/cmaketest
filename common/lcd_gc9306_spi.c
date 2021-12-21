#include "panel_drv.h"
//#include "osa_old_api.h"
//#include <ui_os_api.h>

static int lcd_panel_interface_init(struct panel_spec *self, uint32_t sclk, int32_t work_mode)
{
	struct s_spi_ctx *spi_ctx = NULL;
	struct spi_info * spi = NULL;

	LCDLOGI("INFO: panel_interface_init, spi+++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: panel_interface_init, Invalid param\r\n");
		return -1;
	}

	spi = (struct spi_info *)self->info;
	spi_ctx = spi_init(sclk, spi, work_mode);
	if(NULL == spi_ctx){
		LCDLOGE("ERROR: panel_interface_init, spi init fail!\r\n");
		return -1;
	}
	self->panel_if = (void*)spi_ctx;
	return 0;
}


static int lcd_panel_interface_update(struct panel_spec *self, int32_t work_mode)
{
	int ret;

	if(NULL == self || NULL == self->panel_if){
		LCDLOGE("ERROR: lcd_panel_interface_update, Invalid param\r\n");
		return -1;
	}
	
	ret = spi_update((struct s_spi_ctx *)self->panel_if, work_mode);
	return ret;
}

static int lcd_panel_init_for_BOARD_CRANE_EVB(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
	struct spi_info * spi = NULL;

	if(NULL == self){
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi = (struct spi_info *)self->info;

	//HW_Reset();                
	//while(1){
	LCDLOGE("*******************gc9306 init**************************\r\n");
        mdelay(120);  
        //OSATaskSleep(MS_TO_TICKS(120));
        //---------------------------------end Reset Sequence---------------------------------------------//
        //--------------------------------display control setting----------------------------------------//
        spi_write_cmd(spi_ctx,0xfe,8);
        spi_write_cmd(spi_ctx,0xef,8);
        spi_write_cmd(spi_ctx,0x36,8);
        spi_write_data(spi_ctx,0x48,8);
        spi_write_cmd(spi_ctx,0x3a,8);
        spi_write_data(spi_ctx,0x05,8);
    
 //------------------------------open two lane mode -------------------------------//

        spi_write_cmd(spi_ctx,0xe9,8);
        spi_write_data(spi_ctx,0x08,8); //
    
        //------------------------------end display control setting--------------------------------//
        //------------------------------Power Control Registers Initial------------------------------//
     
        spi_write_cmd(spi_ctx,0xa4,8);
        spi_write_data(spi_ctx,0x44,8);
        spi_write_data(spi_ctx,0x44,8);
        spi_write_cmd(spi_ctx,0xa5,8);
        spi_write_data(spi_ctx,0x42,8);
        spi_write_data(spi_ctx,0x42,8);        
        spi_write_cmd(spi_ctx,0xaa,8);
        spi_write_data(spi_ctx,0x88,8);
        spi_write_data(spi_ctx,0x88,8);
        spi_write_cmd(spi_ctx,0xe8,8);
        spi_write_data(spi_ctx,0x11,8);
        spi_write_data(spi_ctx,0x0b,8);
        spi_write_cmd(spi_ctx,0xe3,8);
        spi_write_data(spi_ctx,0x01,8);
        spi_write_data(spi_ctx,0x10,8);        
        spi_write_cmd(spi_ctx,0xff,8);
        spi_write_data(spi_ctx,0x61,8);
        spi_write_cmd(spi_ctx,0xac,8);
        spi_write_data(spi_ctx,0x00,8);
        spi_write_cmd(spi_ctx,0xad,8);
        spi_write_data(spi_ctx,0x33,8);

        spi_write_cmd(spi_ctx,0xae,8);
        spi_write_data(spi_ctx,0x2b,8);

        spi_write_cmd(spi_ctx,0xAf,8);//DIG_VREFAD_VRDD[2]
        spi_write_data(spi_ctx,0x55,8);

        spi_write_cmd(spi_ctx,0xa6,8);
        spi_write_data(spi_ctx,0x2a,8);
        spi_write_data(spi_ctx,0x2a,8);
        spi_write_cmd(spi_ctx,0xa7,8);
        spi_write_data(spi_ctx,0x2b,8);
        spi_write_data(spi_ctx,0x2b,8);
        spi_write_cmd(spi_ctx,0xa8,8);
        spi_write_data(spi_ctx,0x18,8);
        spi_write_data(spi_ctx,0x18,8);
        spi_write_cmd(spi_ctx,0xa9,8);
        spi_write_data(spi_ctx,0x2a,8);
        spi_write_data(spi_ctx,0x2a,8);       


        //------------------------end Power Control Registers Initial------------------------------//
        //----------------------------display window 240X320------------------------------------//
        spi_write_cmd(spi_ctx,0x2a,8);
        spi_write_data(spi_ctx,0x00,8);
        spi_write_data(spi_ctx,0x00,8);
        spi_write_data(spi_ctx,0x00,8);
        spi_write_data(spi_ctx,0xef,8);
        spi_write_cmd(spi_ctx,0x2b,8);
        spi_write_data(spi_ctx,0x00,8);
        spi_write_data(spi_ctx,0x00,8);
        spi_write_data(spi_ctx,0x01,8);
        spi_write_data(spi_ctx,0x3f,8);
        //spi_write_cmd(spi_ctx,0x2c,8);
        
       //--------end display window --------------//

        spi_write_cmd(spi_ctx,0x35,8);
        spi_write_data(spi_ctx,0x00,8);


        spi_write_cmd(spi_ctx,0x44,8);
        spi_write_data(spi_ctx,0x00,8);
        spi_write_data(spi_ctx,0x0a,8);
  
        //------------gamma setting------------------//
      
        spi_write_cmd(spi_ctx,0xf0,8);
        spi_write_data(spi_ctx,0x02,8);
        spi_write_data(spi_ctx,0x01,8);
        spi_write_data(spi_ctx,0x00,8);
        spi_write_data(spi_ctx,0x03,8);
        spi_write_data(spi_ctx,0x08,8);
        spi_write_data(spi_ctx,0x0d,8);
        spi_write_cmd(spi_ctx,0xf1,8);
        spi_write_data(spi_ctx,0x01,8);
        spi_write_data(spi_ctx,0x01,8);
        spi_write_data(spi_ctx,0x00,8);
        spi_write_data(spi_ctx,0x08,8);
        spi_write_data(spi_ctx,0x11,8);
        spi_write_data(spi_ctx,0x11,8);
        spi_write_cmd(spi_ctx,0xf2,8);
        spi_write_data(spi_ctx,0x0e,8);
        spi_write_data(spi_ctx,0x09,8);
        spi_write_data(spi_ctx,0x37,8);
        spi_write_data(spi_ctx,0x04,8);
        spi_write_data(spi_ctx,0x04,8);
        spi_write_data(spi_ctx,0x48,8);
        spi_write_cmd(spi_ctx,0xf3,8);
        spi_write_data(spi_ctx,0x13,8);
        spi_write_data(spi_ctx,0x0c,8);
        spi_write_data(spi_ctx,0x3f,8);
        spi_write_data(spi_ctx,0x05,8);
        spi_write_data(spi_ctx,0x03,8);
        spi_write_data(spi_ctx,0x4e,8);
        spi_write_cmd(spi_ctx,0xf4,8);
        spi_write_data(spi_ctx,0x0c,8);
        spi_write_data(spi_ctx,0x19,8);
        spi_write_data(spi_ctx,0x18,8);
        spi_write_data(spi_ctx,0x1c,8);
        spi_write_data(spi_ctx,0x1d,8);
        spi_write_data(spi_ctx,0x0F,8);
        spi_write_cmd(spi_ctx,0xf5,8);
        spi_write_data(spi_ctx,0x08,8);
        spi_write_data(spi_ctx,0x14,8);
        spi_write_data(spi_ctx,0x14,8);
        spi_write_data(spi_ctx,0x1a,8);
        spi_write_data(spi_ctx,0x1a,8);
        spi_write_data(spi_ctx,0x0F,8);
        //------------------------------------end gamma setting-----------------------------------------//
        spi_write_cmd(spi_ctx,0x11,8);
        mdelay(120); 
        //OSATaskSleep(MS_TO_TICKS(120));
        spi_write_cmd(spi_ctx,0x29,8);
        spi_write_cmd(spi_ctx,0x2c,8);
          
        return 0;
}


static int lcd_panel_init(struct panel_spec *self)
{
	
    return lcd_panel_init_for_BOARD_CRANE_EVB(self);
	
}

static unsigned int lcd_panel_readid(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
    uint32_t read_id = 0;

	if(NULL == self){
		LCDLOGE("ERROR: panel_readid, Invalid param\r\n");
		return 0;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_readid, spi has not been inited!\r\n");
		return 0;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
#if 0
	spi_read_data(spi_ctx, 0xDA, 8, &read_id, 8);
	
	LCDLOGI("INFO: panel_readid, read 0xDA receive 0x%x!\r\n",read_id);
	
	spi_read_data(spi_ctx, 0xDB, 8, &read_id, 8);
	if(read_id != 0x85){
		LCDLOGE("ERROR: panel_readid, read 0xDB expect 0x85, but receive 0x%x!\r\n",
			read_id);
//		return 0;
	}
	LCDLOGI("INFO: panel_readid, read 0xDB receive 0x%x!\r\n",read_id);

	spi_read_data(spi_ctx, 0xDC, 8, &read_id, 8);
	if(read_id != 0x52){
		LCDLOGE("ERROR: panel_readid, read 0xDC expect 0x52, but receive 0x%x!\r\n",
			read_id);
//		return 0;
	}
	LCDLOGI("INFO: panel_readid, read 0xDC receive 0x%x!\r\n",read_id);
#endif	
    mdelay(200);  	
	spi_read_data(spi_ctx, 0x04, 8, &read_id, 24);

	/*if(read_id != 0x858552){
		LCDLOGE("ERROR: panel_readid, read 0x04 expect 0x9305, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}*/
	LCDLOGI("INFO: panel_readid, read 0x04 receive 0x%x!\r\n",read_id);
 

    return read_id;
}

static int lcd_panel_display_on(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
	LCDLOGI("INFO: lcd_panel_display_on +++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_display_on, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: lcd_panel_display_on, mcu has not been inited!\r\n");
		return -1;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
        spi_write_cmd(spi_ctx, 0xfe, 8);
        spi_write_cmd(spi_ctx, 0xef, 8);
	spi_write_cmd(spi_ctx, 0x11, 8);
	mdelay(120);
	//OSATaskSleep(MS_TO_TICKS(120));
	spi_write_cmd(spi_ctx, 0x29, 8);
	mdelay(20);
	//OSATaskSleep(MS_TO_TICKS(20));
    return 0;
}

static int lcd_panel_display_off(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
	LCDLOGI("INFO: lcd_panel_display_off +++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_display_off, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: lcd_panel_display_off, mcu has not been inited!\r\n");
		return -1;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
        spi_write_cmd(spi_ctx, 0xfe, 8);
        spi_write_cmd(spi_ctx, 0xef, 8);
	spi_write_cmd(spi_ctx, 0x28, 8);
	mdelay(120);
	//OSATaskSleep(MS_TO_TICKS(120));
	spi_write_cmd(spi_ctx, 0x10, 8);
	mdelay(150);
	//OSATaskSleep(MS_TO_TICKS(20));
    return 0;
}

static int lcd_panel_invalid(struct panel_spec *self, uint32_t start_x,
	uint32_t start_y,  uint32_t end_x, uint32_t end_y)
{
	struct s_spi_ctx* spi_ctx = NULL;

	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_invalid, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: lcd_panel_invalid, mcu has not been inited!\r\n");
		return -1;
	}

	LCDLOGI("INFO: lcd_panel_invalid, %d,%d,%d,%d\r\n", start_x, start_y,
		end_x, end_y);

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi_write_cmd(spi_ctx, 0x2A, 8);
	spi_write_data(spi_ctx, ((start_x >> 8) & 0xFF), 8);
	spi_write_data(spi_ctx, (start_x & 0xFF), 8);
	spi_write_data(spi_ctx, ((end_x >> 8) & 0xFF), 8);
	spi_write_data(spi_ctx, (end_x & 0xFF), 8);

	spi_write_cmd(spi_ctx, 0x2B, 8);
	spi_write_data(spi_ctx, ((start_y >> 8) & 0xFF), 8);
	spi_write_data(spi_ctx, (start_y & 0xFF), 8);
	spi_write_data(spi_ctx, ((end_y >> 8) & 0xFF), 8);
	spi_write_data(spi_ctx, (end_y & 0xFF), 8);

	spi_write_cmd(spi_ctx, 0x2C, 8);
	return 0;
}

const static struct panel_operations lcd_gc9306_spi_ops = {
	lcd_panel_interface_init,
	lcd_panel_interface_update,
	lcd_panel_init,
	lcd_panel_invalid,
	lcd_panel_display_off,
	lcd_panel_display_on,
	lcd_panel_readid,
};

const static struct timing_spi lcd_gc9306_spi_timing = {
	52000,//62500, //31200, //26000, // /*kHz*/
	2500, /*kHz*/
};
const static struct spi_info lcd_gc9306_spi_info_for_BOARD_CRANE_EVB = {
	3,  //CSX/SCL/SDA/SDO
	1, 
	2,
	SPI_FORMAT_RGB565,
	0,
	SPI_EDGE_RISING,
	0,
	SPI_ENDIAN_MSB,
	0,
	(struct timing_spi*)&lcd_gc9306_spi_timing,
};

struct panel_spec lcd_gc9306_spi_spec_evb = {
	"gc9306_spi",
	0x9306,
	LCD_CAP_NORMAL,
	240,
	320,
	LCD_MODE_SPI,
	LCD_POLARITY_POS,
	(void*)&lcd_gc9306_spi_info_for_BOARD_CRANE_EVB,
	NULL,
	(struct panel_operations*)&lcd_gc9306_spi_ops,
};

struct panel_spec lcd_gc9304_spi_spec_evb = {
	"gc9304_spi",
	0x9304,
	LCD_CAP_NORMAL,
	240,
	320,
	LCD_MODE_SPI,
	LCD_POLARITY_POS,
	(void*)&lcd_gc9306_spi_info_for_BOARD_CRANE_EVB,
	NULL,
	(struct panel_operations*)&lcd_gc9306_spi_ops,
};


