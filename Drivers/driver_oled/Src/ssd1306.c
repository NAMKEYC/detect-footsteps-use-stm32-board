/**
 *
 * @file       ssd1306.c
 * @date       2022-07-27
 * @author     Nam Nguyen
 *
 */
/* Includes ------------------------------------------------------------------*/
#include "../../Drivers/driver_oled/Inc/ssd1306.h"
/* Private variables -------------------------------------------------------- */
ssd1306_t ssd1306;
static uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];/*create buffer data*/
extern I2C_HandleTypeDef hi2c2;
/* -------------------------------------------------------------- */
uint8_t ssd1306_init(void)
{
	HAL_Delay(1);  /*a little delay to initialize*/
	/* Check if LCD connected to I2C */
	if(HAL_I2C_IsDeviceReady(&hi2c2, SSD1306_I2C_ADDRESS, 1, 2000)!=HAL_OK)
	{
		return 0; //if false
	}
	ssd1306_write_command(0xAE); //display off
	ssd1306_write_command(0x20); //Set Memory Addressing Mode
	ssd1306_write_command(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	ssd1306_write_command(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	ssd1306_write_command(0xC8); //Set COM Output Scan Direction
	ssd1306_write_command(0x00); //---set low column address
	ssd1306_write_command(0x10); //---set high column address
	ssd1306_write_command(0x40); //--set start line address
	ssd1306_write_command(0x81); //--set contrast control register
	ssd1306_write_command(0xFF);
	ssd1306_write_command(0xA1); //--set segment re-map 0 to 127
	ssd1306_write_command(0xA6); //--set normal display
	ssd1306_write_command(0xA8); //--set multiplex ratio(1 to 64)
	ssd1306_write_command(0x3F); //
	ssd1306_write_command(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	ssd1306_write_command(0xD3); //-set display offset
	ssd1306_write_command(0x00); //-not offset
	ssd1306_write_command(0xD5); //--set display clock divide ratio/oscillator frequency
	ssd1306_write_command(0xF0); //--set divide ratio
	ssd1306_write_command(0xD9); //--set pre-charge period
	ssd1306_write_command(0x22); //
	ssd1306_write_command(0xDA); //--set com pins hardware configuration
	ssd1306_write_command(0x12);
	ssd1306_write_command(0xDB); //--set vcomh
	ssd1306_write_command(0x20); //0x20,0.77xVcc
	ssd1306_write_command(0x8D); //--set DC-DC enable
	ssd1306_write_command(0x14); //
	ssd1306_write_command(0xAF); //--display on

	/* Clear screen */
	ssd1306_fill();

	/* Update screen */
	ssd1306_updatescreen();

	/* Set default values */
	ssd1306.currentx = 0;
	ssd1306.currenty = 0;

	/* Initialized OK */
	ssd1306.initialized = 1;

	/* Return OK */
	return 1;
}
void ssd1306_updatescreen(void)
{
	uint8_t m;

	for (m = 0; m < 8; m++)
	{
		ssd1306_write_command(0xB0 + m);
		ssd1306_write_command(0x00);
		ssd1306_write_command(0x10);

		/* Write multi data */
		ssd1306_i2c_writemulti(SSD1306_I2C_ADDRESS, 0x40, &ssd1306_buffer[SSD1306_WIDTH * m], SSD1306_WIDTH);
	}
}
void ssd1306_gotoxy(uint16_t x, uint16_t y)
{
	/* Set write pointers */
	ssd1306.currentx = x;
	ssd1306.currenty = y;
}
void ssd1306_drawpixel(uint16_t x, uint16_t y, color_t color)
{
	if (x >= SSD1306_WIDTH ||y >= SSD1306_HEIGHT)
	{
		/* Error */
		return;
	}

	/* Check if pixels are inverted */
	if (ssd1306.inverted)
	{
		color = (color_t)!color;
	}

	/* Set color */
	if (color == COLOR_WHITE) {
		ssd1306_buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} else {
		ssd1306_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}
char ssd1306_putc(char ch, fontdef_t* Font, color_t color)
{
	uint32_t i, b, j;

	/* Check available space in LCD */
	if (SSD1306_WIDTH <= (ssd1306.currentx + Font->FontWidth) ||SSD1306_HEIGHT <= (ssd1306.currenty + Font->FontHeight))
	{
		/* Error */
		return 0;
	}
	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++)
	{
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++)
		{
			if ((b << j) & 0x8000)
			{
				ssd1306_drawpixel(ssd1306.currentx + j, (ssd1306.currenty + i), (color_t) color);
			} else
			{
				ssd1306_drawpixel(ssd1306.currentx + j, (ssd1306.currenty + i), (color_t)!color);
			}
		}
	}

	/* Increase pointer */
	ssd1306.currentx += Font->FontWidth;

	/* Return character written */
	return ch;
}
char ssd1306_puts(char* str, fontdef_t* Font, color_t color)
{
	/* Write characters */
	while (*str)
	{
		/* Write character by character */
		if (ssd1306_putc(*str, Font, color) != *str)
		{
			/* Return error */
			return *str;
		}

		/* Increase string pointer */
		str++;
	}

	/* Everything OK, zero should be returned */
	return *str;
}
void ssd1306_clear(void)
{
	ssd1306_fill ();     //setting color is a dark color
    ssd1306_updatescreen();
}
void ssd1306_fill() {
	/* Set memory */
	memset(ssd1306_buffer, 0x00, sizeof(ssd1306_buffer));
}
void ssd1306_i2c_writemulti(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count)
{
   uint8_t dt[256];
   dt[0] = reg;
   uint8_t i;
   for(i = 0; i < count; i++)
   {
	   dt[i+1] = data[i];
   }
    HAL_I2C_Master_Transmit(&hi2c2, address, dt, count+1, 10);//transmit 2 byte , 1 byte data consist of multiple data and 1 byte address
}
void ssd1306_write_command(uint8_t command)
{
	uint8_t dt[2];
	dt[0] = 0x00;
	dt[1] = command;
	HAL_I2C_Master_Transmit(&hi2c2, SSD1306_I2C_ADDRESS, dt, 2, 10);//transmit 2 byte , 1 byte data and 1 byte address
}
