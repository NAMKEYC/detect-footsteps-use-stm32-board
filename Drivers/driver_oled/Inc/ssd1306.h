/**
 * @file       ssd1306.h
 * @copyright (C) Alexander Lutsai, 2016
   @Copyright (C) Tilen Majerle, 2015
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2022-07-27
 * @author     Nam Nguyen
 * @brief      Driver for OLED ssd1306 to update information on LCD screen
 * @note
 * @example
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef SSD1306_H_
#define SSD1306_H_

/**
 * @brief I2C address of SSD1306
 */
#ifndef SSD1306_I2C_ADDRESS
#define SSD1306_I2C_ADDRESS      0x78
#endif

/**
 * @bref SSD1306 width in pixels
 */
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH            128
#endif
/**
 * @bref SSD1306 LCD height in pixels
 */
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT           64
#endif

/* Includes ----------------------------------------------------------------- */
#include "stm32f4xx_hal.h"
#include "stdlib.h"
#include "string.h"
#include "../../Drivers/driver_oled/Inc/fonts.h"
/* Private typedef -----------------------------------------------------------*/
/**
 * @brief color enumeration of SSD1306
 */
typedef enum{
	COLOR_BLACK = 0x00, /* Black color, no pixel */
	COLOR_WHITE = 0x01  /* Pixel is set. Color depends on LCD */
}color_t;
/**
 * @brief structure of SSD1306
 */
typedef struct {
	uint16_t currentx;
	uint16_t currenty;
	uint8_t inverted;
	uint8_t initialized;
} ssd1306_t;
/**
 * @brief  Initializes SSD1306 LCD
 * @param  None
 * @retval Initialization status:
 *         - 0: LCD was not detected on I2C port
 *         - >0: LCD initialized OK and ready to use
 */
uint8_t ssd1306_init(void);

/**
 * @brief  Sets cursor pointer to desired location for strings
 * @param  x: X location. This parameter can be a value between 0 and SSD1306_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and SSD1306_HEIGHT - 1
 * @retval None
 */
void ssd1306_gotoxy(uint16_t x, uint16_t y);
/**
 * @brief  Puts character to internal RAM
 * @note   @ref SSD1306_UPDATESCREEN() must be called after that in order to see updated LCD screen
 * @param  ch: Character to be written
 * @param  *Font: Pointer to @ref fontdef_t structure with used font
 * @param  color: Color used for drawing. This parameter can be a value of @ref color enumeration of SSD1306
 * @retval Character written
 */
char ssd1306_putc(char ch, fontdef_t* Font, color_t color);
/**
 * @brief  Puts string to internal RAM
 * @note   @ref SSD1306_UPDATESCREEN() must be called after that in order to see updated LCD screen
 * @param  *str: String to be written
 * @param  *Font: Pointer to @ref fontdef_t structure with used font
 * @param  color: Color used for drawing. This parameter can be a value of @ref color enumeration of SSD1306
 * @retval Zero on success or character value when function failed
 */
char ssd1306_puts(char* str, fontdef_t* Font, color_t color);//
/**
 * @brief  Updates buffer from internal RAM to LCD
 * @note   This function must be called each time you do some changes to LCD, to update buffer from RAM to LCD
 * @param  None
 * @retval None
 */
void ssd1306_updatescreen(void);
/**
 * @brief  clear the display
 * @note   This function will transmit Black color(no pixel) and updated LCD screen when @ref SSD1306_UPDATESCREEN()is called
 * @param  None
 * @retval None
 */
void ssd1306_clear(void);
/**
 * @brief  Fills entire LCD with black color
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  None
 * @retval None
 */
void ssd1306_fill();
/**
 * @brief  Writes multi bytes to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  *data: pointer to data array to write it to slave
 * @param  count: how many bytes will be written
 * @retval None
 */
void ssd1306_i2c_writemulti(uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);
/**
 * @brief  Draws pixel at desired location
 * @note   @ref SSD1306_UpdateScreen() must called after that in order to see updated LCD screen
 * @param  x: X location. This parameter can be a value between 0 and SSD1306_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and SSD1306_HEIGHT - 1
 * @param  color: Color to be used for screen fill. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void ssd1306_drawpixel(uint16_t x, uint16_t y, color_t color);
/**
 * @brief  Writes command with default slave address and reg is 0x00
 * @param  *I2Cx: I2C used
 * @param  command : command to be written
 * @retval None
 */
void ssd1306_write_command(uint8_t command);

#endif
/* End of file -------------------------------------------------------- */
