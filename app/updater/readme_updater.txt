1. Description: 
	How to choose updater*.bin
2. updater*.bin naming rule: updater_lcdtype_otasolution.bin
	lcdtype:
		inlcuding:
            1. support for dkb
            	1.1.support lcd in feature phone dkb           
            		LCD_ADAPT     		   ----> st7789_feature_phone_dkb: 3line_2data; gc9306: 3line_2data; gc9304:3line_2data;
				1.2 support lcd in watch phone dkb
            		LCD_ST7789VER2    	   ----> st7789_watch_dkb: 4line_2data;  
            2. WATCHLCDST7789          ----> 3line_2data
			3. ST7789VER1    		   ----> 3line_1data
			4. GC9306VER1              ----> 3line_1data                                            
            5. GC9306VER2              ----> 4line_1data
            6. ST7789VER2              ----> 4line_2data
            7. NO_LCD                  ----> remove lcd driver in updater.bin
	otasolution:
		inlcuding:
			1.asrsd
			2.adups
			3.redstone
			4.ali
			5.asrspi
	for example:
		updater_lcd_adapt_adups.bin
		lcdtype is lcd_adapt
		ota solution:adups
