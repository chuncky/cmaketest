
//#include "osa.h"
#include "media_clk.h"

struct media_clk_ctx {
	OSMutexRef media_clk_mutex;
	char media_clk_init;
	uint16_t clk_times[MODULE_MAX][SUB_MODULE_MAX];
	uint16_t total_clk_times[SUB_MODULE_MAX];
};
void ui_delay_ms(unsigned int ms){
	ui_delay_us(1000);
}

void ui_delay_us(unsigned int us)
{
	int cnt = 26;//0x100,0x300
	volatile int i = 0;
	while (us--) {
		i = cnt;
		while (i--);
	}
}
#define REG_PMU_BASE 0xD4282800

static unsigned int media_clk_reg_read(unsigned int reg){	
	uint32_t val = 0xBABABABA;	val = BU_REG_READ(REG_PMU_BASE + reg);	
	return val;
}
static void  media_clk_reg_write(unsigned int  reg, unsigned int  val)
{	
    BU_REG_WRITE(REG_PMU_BASE + reg, val);

}
static void media_clk_write_mask(unsigned int  reg, unsigned int  val, unsigned int  mask)
{	uint32_t v;	
    v = media_clk_reg_read(reg);
	v = (v & ~mask) | (val & mask);	
    media_clk_reg_write(reg, v);
}
void media_clk_set_bit(unsigned int  reg, unsigned int  val){	
	media_clk_write_mask(reg, val, val);
}
void media_clk_clr_bit(unsigned int  reg, unsigned int  val){	
	media_clk_write_mask(reg, 0, val);
}

static struct media_clk_ctx g_media_clk_ctx;
static int media_clk_set_rate_isp2x(int clk_rate)
{
	int src, div, val;

	if(clk_rate != 624 && clk_rate != 416 && clk_rate != 312 && clk_rate != 208)
	{
		uart_printf("media_clk_set_rate_isp2x: error clk_rate (%d)\r\n", clk_rate);
		return -1;
	}
	//select isp2x src and div
	if(clk_rate == 624){
		src = 0;
		div = 1;
		val = 0;
	}
	else if(clk_rate == 312){
		src = 0;
		div = 2;
		val = BIT(19);
	}
	else if(clk_rate == 416){
		src = 1;
		div = 1;
		val = BIT(17);
	}
	else{ //208
		src = 1;
		div = 2;
		val = BIT(17)|BIT(19);
	}
	media_clk_write_mask(0x50, val, BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21));
	return 0;

}
static int media_clk_set_rate_offline(int clk_rate)
{
	int src, div, val;

	if(clk_rate != 624 && clk_rate != 416 && clk_rate != 312 && clk_rate != 208)
	{
		uart_printf("media_clk_set_rate_offline: error clk_rate (%d)\r\n", clk_rate);
		return -1;
	}
	//select isp2x src and div
	if(clk_rate == 416){
		src = 0;
		div = 1;
		val = 0;
	}
	if(clk_rate == 208){
		src = 0;
		div = 2;
		val = BIT(27);
	}
	else if(clk_rate == 624){
		src = 1;
		div = 1;
		val = BIT(25);
	}
	else{ // 312
		src = 1;
		div = 2;
		val = BIT(25)|BIT(27);
	}
	media_clk_write_mask(0x50, val, BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29));
	return 0;

}

static int media_clk_set_rate_phy(int clk_rate)
{
	int src, val;

	if(clk_rate != 52 && clk_rate != 104)
	{
		uart_printf("media_clk_set_rate_phy: error clk_rate (%d)\r\n", clk_rate);
		return -1;
	}
	if(clk_rate == 52){
		media_clk_set_bit(0x50, BIT(7));
	}
	else //104
		media_clk_clr_bit(0x50, BIT(7));
	return 0;

}
static int media_clk_set_lcd_ci(int clk_rate)
{
	int src, val;

	if(clk_rate != 312 && clk_rate != 208 && clk_rate != 156 && clk_rate != 350)
	{
		uart_printf("media_clk_set_lcd_ci: error clk_rate (%d)\r\n", clk_rate);
		return -1;
	}
	if(clk_rate == 312){
		val = 0;
	}
	else if(clk_rate == 208){
		val = BIT(10);
	}else if(clk_rate == 156){
		val = BIT(11);
	}else{
		val =BIT(10)|BIT(11);
	}
	media_clk_write_mask(0x50, val, BIT(10)|BIT(11));
	return 0;
}

static int media_clk_set_lcd_sclk(int clk_rate)
{
	switch(clk_rate){
	case 312000:
		media_clk_clr_bit(0x4C, BIT(6));
		media_clk_write_mask(0x4C, 0x0 << 10, 0x1F << 10);
		media_clk_clr_bit(0x4C, BIT(9));
		media_clk_set_bit(0x4C, BIT(8));
		BU_REG_WRITE(0xd420A000 + 0x1A8, 0x40000001);//enable sclk
		break;
	case 125000:
		media_clk_clr_bit(0x4C, BIT(6));
		media_clk_write_mask(0x4C, 0x2 << 10, 0x1F << 10);
		media_clk_set_bit(0x4C, BIT(9));
		media_clk_clr_bit(0x4C, BIT(8));
		BU_REG_WRITE(0xd420A000 + 0x1A8, 0x432D0001);//enable sclk
		break;
	case 104000:
		media_clk_clr_bit(0x4C, BIT(6));
		media_clk_write_mask(0x4C, 0x3 << 10, 0x1F << 10);
		media_clk_set_bit(0x4C, BIT(9));
		media_clk_clr_bit(0x4C, BIT(8));
		BU_REG_WRITE(0xd420A000 + 0x1A8, 0x40000001);//enable sclk
		break;
	case 62400:
		media_clk_clr_bit(0x4C, BIT(6));
		media_clk_write_mask(0x4C, 0x5 << 10, 0x1F << 10);
		media_clk_set_bit(0x4C, BIT(9));
		media_clk_clr_bit(0x4C, BIT(8));
		BU_REG_WRITE(0xd420A000 + 0x1A8, 0x40000001);//enable sclk
		break;
	case 52000:
		media_clk_set_bit(0x4C, BIT(6));
		media_clk_write_mask(0x4C, 0x0 << 10, 0x1F << 10);
		media_clk_clr_bit(0x4C, BIT(9));
		media_clk_set_bit(0x4C, BIT(8));
		BU_REG_WRITE(0xd420A000 + 0x1A8, 0x40000001);//enable sclk
		break;
	default:
		uart_printf("media_clk_set_lcd_sclk: unsupport clk_rate (%d)\r\n", clk_rate);
		return -1;
	}
	return 0;
}

static int media_clk_set_jpeg_func(int clk_rate)
{
	unsigned int val;

	switch (clk_rate) {
		case 156:
			val = BIT(13);
			break;
		case 208:
			val = BIT(12);
			break;
		case 312:
			val = 0;
			break;
		case 350:
			val = BIT(12) | BIT(13);
			break;
		default:
			uart_printf("invalid jpeg func clk %d\r\n", clk_rate);
			return -1;	
	}

	media_clk_write_mask(0x50, val, BIT(12)|BIT(13));
	return 0;
}

int media_clk_set_rate(int clk_type, int clk_rate)
{
	//OSAMutexLock((g_media_clk_ctx.media_clk_mutex), OS_WAIT_FORERVER);
	if(clk_type & MEDIA_CLK_ISP2X){
		media_clk_set_rate_isp2x(clk_rate);
	}
	if(clk_type & MEDIA_CLK_OFFLINE){
		media_clk_set_rate_offline(clk_rate);
	}
	if(clk_type & MEDIA_CLK_PHY){
		media_clk_set_rate_phy(clk_rate);
	}
	if(clk_type & MEDIA_CLK_LCD_CI){
		media_clk_set_lcd_ci(clk_rate);
	}
	if(clk_type & MEDIA_CLK_LCD_SCLK){
		media_clk_set_lcd_sclk(clk_rate);
	}
	if(clk_type & MEDIA_CLK_JPEG_FUNC){
		media_clk_set_jpeg_func(clk_rate);
	}
	//OSAMutexUnlock((g_media_clk_ctx.media_clk_mutex));
	return 0;
}

int media_clk_enable(int clk_type, int module_id)
{
	int error_id = 0;
	int total_clk_error = 0;

	if(module_id > MODULE_VIDEO || module_id < MODULE_CAMERA){
		uart_printf("media_clk_enbale: error module_id [%d]!\r\n", module_id);
		return 0;
	}
	//OSAMutexLock((g_media_clk_ctx.media_clk_mutex), OS_WAIT_FORERVER);
	
	if(clk_type & MEDIA_CLK_ISP2X){
		//clk_times++
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_ISP2X] < 65535){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_ISP2X]++;
		}
		else{
			error_id |= MEDIA_CLK_ISP2X;
		}
		//total_clk_times++
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_ISP2X] < 65535){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_ISP2X]++;
		}
		else{	
			total_clk_error |= MEDIA_CLK_ISP2X;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_ISP2X] == 1)
			media_clk_set_bit(0x50, BIT(16)|BIT(23));
	}
	if(clk_type & MEDIA_CLK_OFFLINE){
		//clk_times++
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE] < 65535){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE]++;
		}
		else{
			error_id |= MEDIA_CLK_OFFLINE;
		}
		//total_clk_times++
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE] < 65535){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE]++;
		}
		else{	
			total_clk_error |= MEDIA_CLK_OFFLINE;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE] == 1)
			media_clk_set_bit(0x50, BIT(24)|BIT(31));
	}
	if(clk_type & MEDIA_CLK_PHY){
		//clk_times++
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_PHY] < 65535){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_PHY]++;
		}
		else{
			error_id |= MEDIA_CLK_PHY;
		}
		//total_clk_times++
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_PHY] < 65535){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_PHY]++;
		}
		else{	
			total_clk_error |= MEDIA_CLK_PHY;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_PHY] == 1)
			media_clk_set_bit(0x50, BIT(2)|BIT(5));
	}
	if(clk_type & MEDIA_CLK_INTERFACE){
		//clk_times++
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_INTERFACE] < 65535){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_INTERFACE]++;
		}
		else{
			error_id |= MEDIA_CLK_INTERFACE;
		}
		//total_clk_times++
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_INTERFACE] < 65535){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_INTERFACE]++;
		}
		else{
			total_clk_error |= MEDIA_CLK_INTERFACE;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_INTERFACE] == 1)
			media_clk_set_bit(0x50, BIT(0)|BIT(3));
	}
	if(clk_type & MEDIA_CLK_OFFLINE_AXI){
		//clk_times++
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE_AXI] < 65535){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE_AXI]++;
		}
		else{
			error_id |= MEDIA_CLK_OFFLINE_AXI;
		}
		//total_clk_times++
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE_AXI] < 65535){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE_AXI]++;
		}
		else{	
			total_clk_error |= MEDIA_CLK_OFFLINE_AXI;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE_AXI] == 1)
			media_clk_set_bit(0x50, BIT(8)|BIT(9));
	}
	if(clk_type & MEDIA_CLK_LCD_CI){
		//clk_times++
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_CI] < 65535){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_CI]++;
		}
		else{
			error_id |= MEDIA_CLK_LCD_CI;
		}
		//total_clk_times++
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_CI] < 65535){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_CI]++;
		}
		else{
			total_clk_error |= MEDIA_CLK_LCD_CI;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_CI] == 1)
			media_clk_set_bit(0x50, BIT(1)|BIT(4));
	}
	if(clk_type & MEDIA_CLK_LCD_AHB){
		//clk_times++
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AHB] < 65535){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AHB]++;
		}
		else{
			error_id |= MEDIA_CLK_LCD_AHB;
		}
		//total_clk_times++
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AHB] < 65535){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AHB]++;
		}
		else{
			total_clk_error |= MEDIA_CLK_LCD_AHB;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AHB]  == 1)
			media_clk_set_bit(0x4C, BIT(2)|BIT(5));
	}
	if(clk_type & MEDIA_CLK_LCD_SCLK){
		//clk_times++
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_SCLK] < 65535){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_SCLK]++;
		}
		else{
			error_id |= MEDIA_CLK_LCD_SCLK;
		}
		//total_clk_times++
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_SCLK] < 65535){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_SCLK]++;
		}
		else{
			total_clk_error |= MEDIA_CLK_LCD_SCLK;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_SCLK]  == 1)
			media_clk_set_bit(0x4C, BIT(4));
			BU_REG_WRITE(0xd420a000 + 0x9c, BU_REG_READ(0xd420a000 + 0x9c) & ~BIT(29));
	}
	if(clk_type & MEDIA_CLK_LCD_AXI){
		//clk_times++
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AXI] < 65535){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AXI]++;
		}
		else{
			error_id |= MEDIA_CLK_LCD_AXI;
		}
		//total_clk_times++
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AXI] < 65535){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AXI]++;
		}
		else{
			total_clk_error |= MEDIA_CLK_LCD_AXI;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AXI]  == 1)
			media_clk_set_bit(0x4C, BIT(0)|BIT(3));
	}
	if(clk_type & MEDIA_CLK_JPEG_FUNC){
		//clk_times++
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_JPEG_FUNC] < 65535){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_JPEG_FUNC]++;
		}
		else{
			error_id |= MEDIA_CLK_JPEG_FUNC;
		}
		//total_clk_times++
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_JPEG_FUNC] < 65535){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_JPEG_FUNC]++;
		}
		else{
			total_clk_error |= MEDIA_CLK_JPEG_FUNC;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_JPEG_FUNC]  == 1)
			media_clk_set_bit(0x50, BIT(14) | BIT(15));
	}

	//OSAMutexUnlock((g_media_clk_ctx.media_clk_mutex));
	if(error_id || total_clk_error){
		uart_printf("media_clk_enable: error, please disable after enable![0x%x][0x%x]\r\n", error_id, total_clk_error);
		return -1;
	}
	return 0;
}

int media_clk_disable(int clk_type, int module_id)
{
	int error_id = 0;
	int total_clk_error = 0;

	if(module_id > MODULE_VIDEO || module_id < MODULE_CAMERA){
		uart_printf("media_clk_disable: error module_id [%d]!\r\n", module_id);
		return 0;
	}
	//OSAMutexLock((g_media_clk_ctx.media_clk_mutex), OS_WAIT_FORERVER);
	if(clk_type & MEDIA_CLK_ISP2X){
		//clk_times--
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_ISP2X] > 0){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_ISP2X]--;
		}
		else{
			error_id |= MEDIA_CLK_ISP2X;
		}
		//total_clk_times--
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_ISP2X] > 0){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_ISP2X]--;
		}
		else{
			total_clk_error |= MEDIA_CLK_ISP2X;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_ISP2X] == 0)
			media_clk_clr_bit(0x50, BIT(16)|BIT(23));
	}
	if(clk_type & MEDIA_CLK_OFFLINE){
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE] > 0){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE]--;
		}
		else{
			error_id |= MEDIA_CLK_OFFLINE;
		}
		//total_clk_times--
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE] > 0){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE]--;
		}
		else{
			total_clk_error |= MEDIA_CLK_OFFLINE;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE] == 0)
			media_clk_clr_bit(0x50, BIT(24)|BIT(31));
	}
	if(clk_type & MEDIA_CLK_PHY){
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_PHY] > 0){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_PHY]--;
		}
		else{
			error_id |= MEDIA_CLK_PHY;
		}
		//total_clk_times--
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_PHY] > 0){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_PHY]--;
		}
		else{
			total_clk_error |= MEDIA_CLK_PHY;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_PHY] == 0)
			media_clk_clr_bit(0x50, BIT(2)|BIT(5));
	}
	if(clk_type & MEDIA_CLK_INTERFACE){
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_INTERFACE] > 0){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_INTERFACE]--;
		}
		else{
			error_id |= MEDIA_CLK_INTERFACE;
		}
		//total_clk_times--
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_INTERFACE] > 0){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_INTERFACE]--;
		}
		else{
			total_clk_error |= MEDIA_CLK_INTERFACE;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_INTERFACE] == 0)
			media_clk_clr_bit(0x50, BIT(0)|BIT(3));
	}
	if(clk_type & MEDIA_CLK_OFFLINE_AXI){
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE_AXI] > 0){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE_AXI]--;
		}
		else{
			error_id |= MEDIA_CLK_OFFLINE_AXI;
		}
		//total_clk_times--
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE_AXI] > 0){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE_AXI]--;
		}
		else{
			total_clk_error |= MEDIA_CLK_OFFLINE_AXI;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE_AXI] == 0)
			media_clk_clr_bit(0x50, BIT(8)|BIT(9));
	}
	if(clk_type & MEDIA_CLK_LCD_CI){
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_CI] > 0){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_CI]--;
		}
		else{
			error_id |= MEDIA_CLK_LCD_CI;
		}
		//total_clk_times--
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_CI] > 0){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_CI]--;
		}
		else{
			total_clk_error |= MEDIA_CLK_LCD_CI;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_CI] == 0)
			media_clk_clr_bit(0x50, BIT(1)|BIT(4));
	}
	if(clk_type & MEDIA_CLK_LCD_AHB){
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AHB] > 0){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AHB]--;
		}
		else{
			error_id |= MEDIA_CLK_LCD_AHB;
		}
		//total_clk_times--
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AHB] > 0){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AHB]--;
		}
		else{
			total_clk_error |= MEDIA_CLK_LCD_AHB;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AHB] == 0)
			media_clk_clr_bit(0x4C, BIT(2)|BIT(5));
	}
	if(clk_type & MEDIA_CLK_LCD_SCLK){
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_SCLK] > 0){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_SCLK]--;
		}
		else{
			error_id |= MEDIA_CLK_LCD_SCLK;
		}
		//total_clk_times--
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_SCLK] > 0){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_SCLK]--;
		}
		else{
			total_clk_error |= MEDIA_CLK_LCD_SCLK;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_SCLK] == 0)
			media_clk_clr_bit(0x4C, BIT(4));
			BU_REG_WRITE(0xd420a000 + 0x9c, BU_REG_READ(0xd420a000 + 0x9c) | BIT(29));
	}
	if(clk_type & MEDIA_CLK_LCD_AXI){
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AXI] > 0){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AXI]--;
		}
		else{
			error_id |= MEDIA_CLK_LCD_AXI;
		}
		//total_clk_times--
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AXI] > 0){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AXI]--;
		}
		else{
			total_clk_error |= MEDIA_CLK_LCD_AXI;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AXI] == 0)
			media_clk_clr_bit(0x4C, BIT(0)|BIT(3));
	}
	if(clk_type & MEDIA_CLK_JPEG_FUNC){
		if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_JPEG_FUNC] > 0){
			g_media_clk_ctx.clk_times[module_id][SUB_MODULE_JPEG_FUNC]--;
		}
		else{
			error_id |= MEDIA_CLK_JPEG_FUNC;
		}
		//total_clk_times--
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_JPEG_FUNC] > 0){
			g_media_clk_ctx.total_clk_times[SUB_MODULE_JPEG_FUNC]--;
		}
		else{
			total_clk_error |= MEDIA_CLK_JPEG_FUNC;
		}
		if(g_media_clk_ctx.total_clk_times[SUB_MODULE_JPEG_FUNC] == 0)
			media_clk_clr_bit(0x50, BIT(14) | BIT(15));
	}

	//OSAMutexUnlock((g_media_clk_ctx.media_clk_mutex));
	if(error_id || total_clk_error){
		uart_printf("media_clk_disable: error, please enable before disable![%d][%d]\r\n", error_id,total_clk_error);
	}
	return 0;
}

int media_power_on(int module_id)
{
	if(module_id > MODULE_VIDEO || module_id < MODULE_CAMERA){
		uart_printf("media_power_on: error module_id [%d]!\r\n", module_id);
		return -1;
	}
	if(g_media_clk_ctx.media_clk_init == 0){
		//OSAMutexCreate(&(g_media_clk_ctx.media_clk_mutex),OS_PRIORITY);
		g_media_clk_ctx.media_clk_init = 1;
	}
	//OSAMutexLock((g_media_clk_ctx.media_clk_mutex), OS_WAIT_FORERVER);
	// clk_times++
	if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_POWER] < 65535)
		g_media_clk_ctx.clk_times[module_id][SUB_MODULE_POWER]++;
	else{
		uart_printf("media_power_on: clk_times error[%d], please uninit after init!\r\n", (int)SUB_MODULE_POWER);
	}
	// total_clk_times++
	if(g_media_clk_ctx.total_clk_times[SUB_MODULE_POWER] < 65535)
		g_media_clk_ctx.total_clk_times[SUB_MODULE_POWER]++;
	else{
		uart_printf("media_power_on: total_clk_times error[%d], please uninit after init!\n", (int)SUB_MODULE_POWER);
	}
	if(g_media_clk_ctx.total_clk_times[SUB_MODULE_POWER] == 1){
		//enblae Multimedia Power Switch
		media_clk_set_bit(0x4c, BIT(17));
		ui_delay_ms(1);
		media_clk_set_bit(0x4c, BIT(18));
		ui_delay_ms(1);
		media_clk_set_bit(0x4c, BIT(16));
		//release Multimedia Power reset
		media_clk_set_bit(0x4c, BIT(1));
#if 1
		//set rate for all isp clock
		media_clk_set_rate(MEDIA_CLK_ISP2X, 312);
		media_clk_set_rate(MEDIA_CLK_PHY, 52);
		media_clk_set_rate(MEDIA_CLK_OFFLINE, 312);
		media_clk_set_rate(MEDIA_CLK_LCD_CI, 312);
		media_clk_set_rate(MEDIA_CLK_JPEG_FUNC, 312);
#endif
	}
	//OSAMutexUnlock((g_media_clk_ctx.media_clk_mutex));

	return 0;
}

int media_power_off(int module_id)
{
	if(module_id > MODULE_VIDEO || module_id < MODULE_CAMERA){
		uart_printf("media_power_off: error module_id [%d]!\n", module_id);
		return -1;
	}
	//OSAMutexLock((g_media_clk_ctx.media_clk_mutex), OS_WAIT_FORERVER);
	// clk_times--
	if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_POWER] > 0)
		g_media_clk_ctx.clk_times[module_id][SUB_MODULE_POWER]--;
	else{
		uart_printf("media_power_off: clk_times error[%d], please init before uninit!\n", (int)SUB_MODULE_POWER);
	}
	// total_clk_times--
	if(g_media_clk_ctx.total_clk_times[SUB_MODULE_POWER] > 0)
		g_media_clk_ctx.total_clk_times[SUB_MODULE_POWER]--;
	else{
		uart_printf("media_power_off: total_clk_times error[%d], please init before uninit!\n", (int)SUB_MODULE_POWER);
	}
	if(g_media_clk_ctx.total_clk_times[SUB_MODULE_POWER] == 0){
		//disable Multimedia Power Switch
		media_clk_clr_bit(0x4c, BIT(16));
		media_clk_clr_bit(0x4c, BIT(18));
		media_clk_clr_bit(0x4c, BIT(17));
		//enable Multimedia Power reset
		media_clk_clr_bit(0x4c, BIT(1));
	}
	//OSAMutexUnlock((g_media_clk_ctx.media_clk_mutex));
	return 0;

}

int media_clk_dump(enum media_clk_dump_level dump_level)
{
	unsigned int val1, val2;
	val1 = media_clk_reg_read(0x4c);
	val2 = media_clk_reg_read(0x50);
	uart_printf("media_clk_dump:0x4c:0x%x,0x50:0x%x\n", val1,val2);
	if(dump_level == LEVEL_DUMP_CLOCK_TIMES){
		for(val1 = 0;val1 < MODULE_MAX; val1++){
			for(val2 = 0;val2 < SUB_MODULE_MAX; val2++){
				uart_printf("media_clk_dump: clk_times[%d][%d] = %d\n", val1,val2, g_media_clk_ctx.clk_times[val1][val2]);
			}
		}
		for(val1 = 0;val1 < MODULE_MAX; val1++){
			uart_printf("media_clk_dump: total_clk_times[%d] = %d\n", val1, g_media_clk_ctx.total_clk_times[val1]);
		}
	}
	return 0;
}


