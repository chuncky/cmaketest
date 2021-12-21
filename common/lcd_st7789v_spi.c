#include "panel_drv.h"

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

	LCDLOGI("lcd_panel_init_for_BOARD_CRANE_EVB ++\r\n");

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


    mdelay(120);                
	
	spi_write_cmd(spi_ctx, 0x11, 8);     
	
	mdelay(120);
	//OSATaskSleep(MS_TO_TICKS(120));
	
	spi_write_cmd(spi_ctx, 0x36, 8);     
	spi_write_data(spi_ctx, 0x00, 8);   
	
	spi_write_cmd(spi_ctx, 0x3A, 8);     
	spi_write_data(spi_ctx, 0x06, 8);   
	
	spi_write_cmd(spi_ctx, 0x35, 8);     
	spi_write_data(spi_ctx, 0x00, 8);   
	
	spi_write_cmd(spi_ctx, 0xC7, 8);     
	spi_write_data(spi_ctx, 0x00, 8);   
	
	spi_write_cmd(spi_ctx, 0xCC, 8);     
	spi_write_data(spi_ctx, 0x09, 8);   
	
	spi_write_cmd(spi_ctx, 0xB2, 8);     
	spi_write_data(spi_ctx, 0x0C, 8);   
	spi_write_data(spi_ctx, 0x0C, 8);   
	spi_write_data(spi_ctx, 0x00, 8);   
	spi_write_data(spi_ctx, 0x33, 8);   
	spi_write_data(spi_ctx, 0x33, 8);  

#if 1
	if(spi->format == SPI_FORMAT_RGB666){
        spi_write_cmd(spi_ctx, 0x3a, 8);
		spi_write_data(spi_ctx, 0x06, 8); //format is RGB666
    }
	else{
        spi_write_cmd(spi_ctx, 0x3a, 8);
		spi_write_data(spi_ctx, 0x05, 8); //format is RGB565
    }
//#if ( defined ST7789_3WIRE_2LANE_SPI_LANDSCAPE )
//		spi_write_cmd(spi_ctx, 0x36, 8);
//		spi_write_data(spi_ctx, 0xA0, 8);
//#else
    spi_write_cmd(spi_ctx, 0x36, 8);
    spi_write_data(spi_ctx, 0x00, 8); //format is RGB666
//#endif    
#endif

	spi_write_cmd(spi_ctx, 0xB7, 8);     
	spi_write_data(spi_ctx, 0x35, 8);   //vgh=13.26 vgl=-12.5
	
	spi_write_cmd(spi_ctx, 0xBB, 8);     
	spi_write_data(spi_ctx, 0x36, 8);   
	
	spi_write_cmd(spi_ctx, 0xC0, 8);     
	spi_write_data(spi_ctx, 0x2C, 8);   
	
	spi_write_cmd(spi_ctx, 0xC2, 8);     
	spi_write_data(spi_ctx, 0x01, 8);   
	
	spi_write_cmd(spi_ctx, 0xC3, 8);     
	spi_write_data(spi_ctx, 0x0D, 8);   
	
	spi_write_cmd(spi_ctx, 0xC4, 8);     
	spi_write_data(spi_ctx, 0x20, 8);   
	
	spi_write_cmd(spi_ctx, 0xC6, 8);     
	spi_write_data(spi_ctx, 0x15, 8);   //50hz
	
	spi_write_cmd(spi_ctx, 0xD0, 8);     
	spi_write_data(spi_ctx, 0xA4, 8);   
	spi_write_data(spi_ctx, 0xA1, 8);   
	
	spi_write_cmd(spi_ctx, 0xE0, 8);     
	spi_write_data(spi_ctx, 0xD0, 8);   
	spi_write_data(spi_ctx, 0x17, 8);   
	spi_write_data(spi_ctx, 0x19, 8);   
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x03, 8);   
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x32, 8);   
	spi_write_data(spi_ctx, 0x41, 8);   
	spi_write_data(spi_ctx, 0x43, 8);   
	spi_write_data(spi_ctx, 0x09, 8);   
	spi_write_data(spi_ctx, 0x14, 8);   
	spi_write_data(spi_ctx, 0x12, 8);   
	spi_write_data(spi_ctx, 0x33, 8);   
	spi_write_data(spi_ctx, 0x2C, 8);   
	
	spi_write_cmd(spi_ctx, 0xE1, 8);     
	spi_write_data(spi_ctx, 0xD0, 8);   
	spi_write_data(spi_ctx, 0x18, 8);   
	spi_write_data(spi_ctx, 0x17, 8);   
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x03, 8);   
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x31, 8);   
	spi_write_data(spi_ctx, 0x46, 8);   
	spi_write_data(spi_ctx, 0x43, 8);   
	spi_write_data(spi_ctx, 0x09, 8);   
	spi_write_data(spi_ctx, 0x14, 8);   
	spi_write_data(spi_ctx, 0x13, 8);   
	spi_write_data(spi_ctx, 0x31, 8);   
	spi_write_data(spi_ctx, 0x2D, 8);   

	if(spi->data_lane_num == 2){
		spi_write_cmd(spi_ctx, 0xE7, 8);
        spi_write_data(spi_ctx, 0x11, 8);
	}
	
	spi_write_cmd(spi_ctx, 0x29, 8);

	
	return 0;
}

static int lcd_panel_init_for_BOARD_CRANE_PHONE(struct panel_spec *self)
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
	
	spi_write_cmd(spi_ctx, 0x11, 8);
	mdelay(120);                
	//OSATaskSleep(MS_TO_TICKS(120));

	spi_write_cmd(spi_ctx, 0x36, 8);
	spi_write_data(spi_ctx, 0x00, 8);
	
	spi_write_cmd(spi_ctx, 0x3A, 8);
	spi_write_data(spi_ctx, 0x05, 8);

	spi_write_cmd(spi_ctx, 0xB2, 8);
	spi_write_data(spi_ctx, 0x20, 8);
	spi_write_data(spi_ctx, 0x20, 8);
	spi_write_data(spi_ctx, 0x00, 8);
	spi_write_data(spi_ctx, 0x33, 8);
	spi_write_data(spi_ctx, 0x33, 8);

	spi_write_cmd(spi_ctx, 0xB7, 8);
	spi_write_data(spi_ctx, 0x71, 8);

	spi_write_cmd(spi_ctx, 0xBB, 8);
	spi_write_data(spi_ctx, 0x37, 8);

	spi_write_cmd(spi_ctx, 0xC0, 8);
	spi_write_data(spi_ctx, 0x2C, 8);

	spi_write_cmd(spi_ctx, 0xC2, 8);
	spi_write_data(spi_ctx, 0x01, 8);

	spi_write_cmd(spi_ctx, 0xC3, 8);
	spi_write_data(spi_ctx, 0x17, 8);

	spi_write_cmd(spi_ctx, 0xC4, 8);
	spi_write_data(spi_ctx, 0x20, 8);

	spi_write_cmd(spi_ctx, 0xC5, 8);
	spi_write_data(spi_ctx, 0x1F, 8);

	spi_write_cmd(spi_ctx, 0xC6, 8);
	spi_write_data(spi_ctx, 0x0F, 8);

	spi_write_cmd(spi_ctx, 0xD0, 8);
	spi_write_data(spi_ctx, 0xA4, 8);
	spi_write_data(spi_ctx, 0x81, 8);

	spi_write_cmd(spi_ctx, 0xE0, 8);
	spi_write_data(spi_ctx, 0xD0, 8);
	spi_write_data(spi_ctx, 0x08, 8);
	spi_write_data(spi_ctx, 0x12, 8);
	spi_write_data(spi_ctx, 0x0F, 8);
	spi_write_data(spi_ctx, 0x10, 8);
	spi_write_data(spi_ctx, 0x0A, 8);
	spi_write_data(spi_ctx, 0x39, 8);
	spi_write_data(spi_ctx, 0x54, 8);
	spi_write_data(spi_ctx, 0x4A, 8);
	spi_write_data(spi_ctx, 0x2B, 8);
	spi_write_data(spi_ctx, 0x17, 8);
	spi_write_data(spi_ctx, 0x16, 8);
	spi_write_data(spi_ctx, 0x1B, 8);
	spi_write_data(spi_ctx, 0x1D, 8);
	
	spi_write_cmd(spi_ctx, 0xE1, 8);
	spi_write_data(spi_ctx, 0xD0, 8);
	spi_write_data(spi_ctx, 0x08, 8);
	spi_write_data(spi_ctx, 0x11, 8);
	spi_write_data(spi_ctx, 0x0E, 8);
	spi_write_data(spi_ctx, 0x0F, 8);
	spi_write_data(spi_ctx, 0x0A, 8);
	spi_write_data(spi_ctx, 0x39, 8);
	spi_write_data(spi_ctx, 0x44, 8);
	spi_write_data(spi_ctx, 0x4A, 8);
	spi_write_data(spi_ctx, 0x2B, 8);
	spi_write_data(spi_ctx, 0x17, 8);
	spi_write_data(spi_ctx, 0x16, 8);
	spi_write_data(spi_ctx, 0x1B, 8);
	spi_write_data(spi_ctx, 0x1E, 8);

	if(spi->data_lane_num == 2){
		spi_write_cmd(spi_ctx, 0xE7, 8);
        spi_write_data(spi_ctx, 0x11, 8);
	}

	spi_write_cmd(spi_ctx, 0x35, 8);
	spi_write_data(spi_ctx, 0x00, 8);

	spi_write_cmd(spi_ctx, 0x29, 8);
	//OSATaskSleep(20*32);
	mdelay(20*32);
	
	return 0;
}

static int lcd_panel_init_for_BOARD_CRANE_WATCH(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
	struct spi_info * spi = NULL;

	LCDLOGE("lcd_panel_init_for_BOARD_CRANE_WATCH ++\r\n");

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


    mdelay(120);                
	
	spi_write_cmd(spi_ctx, 0x11, 8);     
	
	mdelay(120);
	//OSATaskSleep(MS_TO_TICKS(120));
	
	spi_write_cmd(spi_ctx, 0x36, 8);     
	spi_write_data(spi_ctx, 0x00, 8);   
	
	spi_write_cmd(spi_ctx, 0x3A, 8);     
	spi_write_data(spi_ctx, 0x05, 8);   
	
	spi_write_cmd(spi_ctx, 0xB2, 8); 

	spi_write_data(spi_ctx, 0x2A, 8);   
	spi_write_data(spi_ctx, 0x2C, 8);

	spi_write_data(spi_ctx, 0x00, 8);   
	spi_write_data(spi_ctx, 0x33, 8);   
	spi_write_data(spi_ctx, 0x33, 8);  

#if 1
	if(spi->format == SPI_FORMAT_RGB666){
        spi_write_cmd(spi_ctx, 0x3a, 8);
		spi_write_data(spi_ctx, 0x06, 8); //format is RGB666
    }
	else{
        spi_write_cmd(spi_ctx, 0x3a, 8);
		spi_write_data(spi_ctx, 0x05, 8); //format is RGB565
    }

    spi_write_cmd(spi_ctx, 0x36, 8);
    spi_write_data(spi_ctx, 0x00, 8); //format is RGB666
#endif

	spi_write_cmd(spi_ctx, 0xB7, 8); 

	spi_write_data(spi_ctx, 0x75, 8);   //vgh=13.26 vgl=-12.5
	
	spi_write_cmd(spi_ctx, 0xBB, 8);     
	spi_write_data(spi_ctx, 0x3d, 8);  


	spi_write_cmd(spi_ctx, 0xC0, 8);     
	spi_write_data(spi_ctx, 0x2C, 8);   
	
	spi_write_cmd(spi_ctx, 0xC2, 8);     
	spi_write_data(spi_ctx, 0x01, 8);   
	
	spi_write_cmd(spi_ctx, 0xC3, 8); 

	spi_write_data(spi_ctx, 0x19, 8); 



	spi_write_cmd(spi_ctx, 0xC4, 8);     
	spi_write_data(spi_ctx, 0x20, 8);   
	
	spi_write_cmd(spi_ctx, 0xC6, 8);     
	//spi_write_data(spi_ctx, 0x15, 8);   //50hz
	spi_write_data(spi_ctx, 0x1d, 8);   //44hz
	
	spi_write_cmd(spi_ctx, 0xD0, 8);     
	spi_write_data(spi_ctx, 0xA7, 8);   
	spi_write_data(spi_ctx, 0xA1, 8);   
	
	spi_write_cmd(spi_ctx, 0xE0, 8);     
	spi_write_data(spi_ctx, 0x70, 8);      
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x08, 8);   
	spi_write_data(spi_ctx, 0x09, 8);   
   
	spi_write_data(spi_ctx, 0x09, 8);   
	spi_write_data(spi_ctx, 0x05, 8);   
	spi_write_data(spi_ctx, 0x2A, 8);   
	spi_write_data(spi_ctx, 0x33, 8); 
	//
	spi_write_data(spi_ctx, 0x41, 8);   
	spi_write_data(spi_ctx, 0x07, 8);   
	spi_write_data(spi_ctx, 0x13, 8);   
	spi_write_data(spi_ctx, 0x13, 8);
	
	spi_write_data(spi_ctx, 0x29, 8);   
	spi_write_data(spi_ctx, 0x2F, 8); 
	
	spi_write_cmd(spi_ctx, 0xE1, 8);     
	spi_write_data(spi_ctx, 0x70, 8);   
  
	spi_write_data(spi_ctx, 0x03, 8);   
	spi_write_data(spi_ctx, 0x09, 8);   
	spi_write_data(spi_ctx, 0x0A, 8);   
   
	spi_write_data(spi_ctx, 0x09, 8); 
	//
	spi_write_cmd(spi_ctx, 0x06, 8);     
	spi_write_data(spi_ctx, 0x2B, 8);   
  
	spi_write_data(spi_ctx, 0x34, 8);   
	spi_write_data(spi_ctx, 0x41, 8);   
	spi_write_data(spi_ctx, 0x07, 8);   
   
	spi_write_data(spi_ctx, 0x12, 8); 
	
	spi_write_data(spi_ctx, 0x14, 8);   
	spi_write_data(spi_ctx, 0x2B, 8); 
	spi_write_data(spi_ctx, 0x2E, 8);
	//
	spi_write_cmd(spi_ctx, 0xE4, 8);
	
	spi_write_data(spi_ctx, 0x1d, 8); 
	spi_write_cmd(spi_ctx, 0x35, 8);		//LPTE ON
	spi_write_data(spi_ctx, 0x00, 8); 

	//
	spi_write_cmd(spi_ctx, 0x44, 8);		
	spi_write_data(spi_ctx, 0x00, 8);	//0
	spi_write_data(spi_ctx, 0x00, 8);
	

	spi_write_cmd(spi_ctx, 0x21, 8);

	spi_write_cmd(spi_ctx, 0x2A, 8);	//column Address Set
    spi_write_data(spi_ctx, 0x00, 8);

	//
	spi_write_data(spi_ctx, 0x00, 8);	//0
	spi_write_data(spi_ctx, 0x00, 8);	
	spi_write_data(spi_ctx, 0xEF, 8);
	spi_write_cmd(spi_ctx, 0x2B, 8);

	spi_write_data(spi_ctx, 0x00, 8);
	spi_write_data(spi_ctx, 0x00, 8);	//0
	spi_write_data(spi_ctx, 0x00, 8);	
	spi_write_data(spi_ctx, 0xEF, 8);
	
	spi_write_cmd(spi_ctx, 0x2C, 8);
	spi_write_cmd(spi_ctx, 0x29, 8);

	
	return 0;
}

static int lcd_panel_init_for_BOARD_CRANE_WATCH_1(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
	struct spi_info * spi = NULL;

	LCDLOGE("lcd_panel_init_for_BOARD_CRANE_WATCH ++\r\n");

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


    mdelay(120);                
	
	spi_write_cmd(spi_ctx, 0x11, 8);     
	
	mdelay(120);
	//OSATaskSleep(MS_TO_TICKS(120));
	
	spi_write_cmd(spi_ctx, 0x36, 8);     
	spi_write_data(spi_ctx, 0x00, 8);   
	
	spi_write_cmd(spi_ctx, 0x3A, 8);     
	spi_write_data(spi_ctx, 0x05, 8);   
	
	spi_write_cmd(spi_ctx, 0x35, 8);     
	spi_write_data(spi_ctx, 0x00, 8);   
	
	spi_write_cmd(spi_ctx, 0xC7, 8);     
	spi_write_data(spi_ctx, 0x00, 8);   
	
	spi_write_cmd(spi_ctx, 0xCC, 8);     
	spi_write_data(spi_ctx, 0x09, 8);   
	
	spi_write_cmd(spi_ctx, 0xB2, 8);     
	spi_write_data(spi_ctx, 0x0C, 8);   
	spi_write_data(spi_ctx, 0x0C, 8);   
	spi_write_data(spi_ctx, 0x00, 8);   
	spi_write_data(spi_ctx, 0x33, 8);   
	spi_write_data(spi_ctx, 0x33, 8);  

#if 1
	if(spi->format == SPI_FORMAT_RGB666){
        spi_write_cmd(spi_ctx, 0x3a, 8);
		spi_write_data(spi_ctx, 0x06, 8); //format is RGB666
    }
	else{
        spi_write_cmd(spi_ctx, 0x3a, 8);
		spi_write_data(spi_ctx, 0x05, 8); //format is RGB565
    }

    spi_write_cmd(spi_ctx, 0x36, 8);
    spi_write_data(spi_ctx, 0x00, 8); //format is RGB666
#endif

	spi_write_cmd(spi_ctx, 0xB7, 8);     
	spi_write_data(spi_ctx, 0x35, 8);   //vgh=13.26 vgl=-12.5
	
	spi_write_cmd(spi_ctx, 0xBB, 8);     
	spi_write_data(spi_ctx, 0x36, 8);   
	
	spi_write_cmd(spi_ctx, 0xC0, 8);     
	spi_write_data(spi_ctx, 0x2C, 8);   
	
	spi_write_cmd(spi_ctx, 0xC2, 8);     
	spi_write_data(spi_ctx, 0x01, 8);   
	
	spi_write_cmd(spi_ctx, 0xC3, 8);     
	spi_write_data(spi_ctx, 0x0D, 8);   
	
	spi_write_cmd(spi_ctx, 0xC4, 8);     
	spi_write_data(spi_ctx, 0x20, 8);   
	
	spi_write_cmd(spi_ctx, 0xC6, 8);     
	//spi_write_data(spi_ctx, 0x15, 8);   //50hz
	spi_write_data(spi_ctx, 0x0F, 8);   //50hz
	
	spi_write_cmd(spi_ctx, 0xD0, 8);     
	spi_write_data(spi_ctx, 0xA4, 8);   
	spi_write_data(spi_ctx, 0xA1, 8);   
	
	spi_write_cmd(spi_ctx, 0xE0, 8);     
	spi_write_data(spi_ctx, 0xD0, 8);   
	spi_write_data(spi_ctx, 0x17, 8);   
	spi_write_data(spi_ctx, 0x19, 8);   
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x03, 8);   
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x32, 8);   
	spi_write_data(spi_ctx, 0x41, 8);   
	spi_write_data(spi_ctx, 0x43, 8);   
	spi_write_data(spi_ctx, 0x09, 8);   
	spi_write_data(spi_ctx, 0x14, 8);   
	spi_write_data(spi_ctx, 0x12, 8);   
	spi_write_data(spi_ctx, 0x33, 8);   
	spi_write_data(spi_ctx, 0x2C, 8);   
	
	spi_write_cmd(spi_ctx, 0xE1, 8);     
	spi_write_data(spi_ctx, 0xD0, 8);   
	spi_write_data(spi_ctx, 0x18, 8);   
	spi_write_data(spi_ctx, 0x17, 8);   
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x03, 8);   
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x31, 8);   
	spi_write_data(spi_ctx, 0x46, 8);   
	spi_write_data(spi_ctx, 0x43, 8);   
	spi_write_data(spi_ctx, 0x09, 8);   
	spi_write_data(spi_ctx, 0x14, 8);   
	spi_write_data(spi_ctx, 0x13, 8);   
	spi_write_data(spi_ctx, 0x31, 8);   
	spi_write_data(spi_ctx, 0x2D, 8);   

	if(spi->data_lane_num == 2){
		spi_write_cmd(spi_ctx, 0xE7, 8);
        spi_write_data(spi_ctx, 0x11, 8);
	}
	
	spi_write_cmd(spi_ctx, 0x29, 8);

	
	return 0;
}


static int lcd_panel_init(struct panel_spec *self)
{

#ifdef WATCHLCDST7789_CODE_USE	
	return lcd_panel_init_for_BOARD_CRANE_WATCH(self);		
#endif

#ifdef LCD_ST7789VER2	
	return lcd_panel_init_for_BOARD_CRANE_WATCH_1(self);		
#endif

#if (defined LCD_ADAPT) || (defined LCD_ST7789VER1) || ( defined ST7789_3WIRE_2LANE_SPI_LANDSCAPE )
	return lcd_panel_init_for_BOARD_CRANE_EVB(self);
#endif
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
#if 1 
	spi_read_data(spi_ctx, 0xDA, 8, &read_id, 8);
	
	LCDLOGI("INFO: panel_readid, read 0xDA receive 0x%x!\r\n",read_id);
	
	spi_read_data(spi_ctx, 0xDB, 8, &read_id, 8);
	if(read_id != 0x85){
//		LCDLOGE("ERROR: panel_readid, read 0xDB expect 0x85, but receive 0x%x!\r\n",
//			read_id);
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
	spi_write_cmd(spi_ctx, 0x11, 8);
	//OSATaskSleep(MS_TO_TICKS(120));
	mdelay(120);
	spi_write_cmd(spi_ctx, 0x29, 8);
	//OSATaskSleep(MS_TO_TICKS(20));
	mdelay(20);
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
	spi_write_cmd(spi_ctx, 0x28, 8);
	//OSATaskSleep(MS_TO_TICKS(20));
	mdelay(20);
	spi_write_cmd(spi_ctx, 0x10, 8);
	//OSATaskSleep(MS_TO_TICKS(120));
	mdelay(120);
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

const static struct panel_operations lcd_st7789v_spi_ops = {
	lcd_panel_interface_init,
	lcd_panel_interface_update,
	lcd_panel_init,
	lcd_panel_invalid,
	lcd_panel_display_off,
	lcd_panel_display_on,
	lcd_panel_readid,
};

const static struct timing_spi lcd_st7789v_spi_timing = {
	52000,//62500, //31200, //26000, // /*kHz*/
	2500, /*kHz*/
};

const static struct spi_info lcd_st7789v_spi_info_for_BOARD_CRANE_PHONE = {
	4,
	2, 
	1,
	SPI_FORMAT_RGB565,
	0,
	SPI_EDGE_RISING,
	0,
	SPI_ENDIAN_MSB,
	0,
	(struct timing_spi*)&lcd_st7789v_spi_timing,
};

#if	(defined WATCHLCDST7789_CODE_USE) || (defined LCD_ST7789VER2)
const static struct spi_info lcd_st7789v_spi_info_watch = {
	4,
	1,
#ifdef WATCHLCDST7789_CODE_USE
	1,		//data_lane_num
#endif
#ifdef LCD_ST7789VER2
	2,		//data_lane_num
#endif	
	SPI_FORMAT_RGB565,
	0,
	SPI_EDGE_RISING,
	0,		//360
	SPI_ENDIAN_MSB,
	0,
	&lcd_st7789v_spi_timing,
};
#endif


const static struct spi_info lcd_st7789v_spi_info_for_BOARD_CRANE_EVB = {
	3,  //CSX/SCL/SDA/SDO
	1, 
#ifdef LCD_ST7789VER1	
	1,
#else					
	2,
#endif
	SPI_FORMAT_RGB565,
	0,
	SPI_EDGE_RISING,
	0,
	SPI_ENDIAN_MSB,
	0,
	(struct timing_spi*)&lcd_st7789v_spi_timing,
};

struct panel_spec lcd_st7789v_spi_spec_phone = {
	"st7789v_spi",
	0x858552,
	LCD_CAP_NORMAL,
	240,
	320,
	LCD_MODE_SPI,
	LCD_POLARITY_POS,
	(void*)&lcd_st7789v_spi_info_for_BOARD_CRANE_PHONE,
	NULL,
	(struct panel_operations*)&lcd_st7789v_spi_ops,
};

#if (defined WATCHLCDST7789_CODE_USE) || (defined LCD_ST7789VER2) 
struct panel_spec lcd_st7789v_spi_spec_watch = {
	"st7789v_spi",
	0x858552,
	LCD_CAP_NORMAL,
	240,
	240,
	LCD_MODE_SPI,
	LCD_POLARITY_POS,
	(void*)&lcd_st7789v_spi_info_watch,
	NULL,
	(struct panel_operations*)&lcd_st7789v_spi_ops,
};		
#endif

#define SIMU_LCD_WDITH  (128)
#define SIMU_LCD_HEIGHT (160)

struct panel_spec lcd_st7789v_spi_spec_evb = {
	"st7789v_spi",
	0x858552,
	LCD_CAP_NORMAL,
#if ( defined SIMU_128_160_ON_LCD240x320 )
	SIMU_LCD_WDITH,
	SIMU_LCD_HEIGHT,
//#elif ( defined ST7789_3WIRE_2LANE_SPI_LANDSCAPE )
//	320,
//	240,
#else
	240,
	320,
#endif	
	LCD_MODE_SPI,
	LCD_POLARITY_POS,
	(void*)&lcd_st7789v_spi_info_for_BOARD_CRANE_EVB,
	NULL,
	(struct panel_operations*)&lcd_st7789v_spi_ops,
};

