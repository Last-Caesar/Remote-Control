#include "st7789v0.h"
#include <string.h>
uint16_t DMA_MIN_SIZE = 16;
uint16_t x_shift = 0;
uint16_t y_shift = 0;
/**
 * @brief Write command to ST7789 controller
 * @param cmd -> command to write
 * @return none
 */
static void ST7789_WriteCommand(uint8_t cmd);
static void ST7789_WriteData(uint8_t *buff, size_t buff_size);
static void ST7789_WriteSmallData(uint8_t data);
static void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void ST7789_ExecuteCommandList(const uint8_t *addr);

static const uint8_t init_cmds[] = {
	9,
	// 9 commands in list:
	ST7789_SWRESET,
	DELAY,
	// 1: Software reset, no args, w/delay
	150,
	//    150 ms delay
	ST7789_SLPOUT,
	DELAY,
	// 2: Out of sleep mode, no args, w/delay
	255,
	//    255 = 500 ms delay
	ST7789_COLMOD,
	1 + DELAY,
	// 3: Set color mode, 1 arg + delay:
	(ST7789_ColorMode_65K | ST7789_ColorMode_16bit),
	//    16-bit color 0x55
	10,
	//    10 ms delay
	ST7789_MADCTL,
	1,
	// 4: Memory access ctrl (directions), 1 arg:
	ST7789_ROTATION,
	//    Row addr/col addr, bottom to top refresh
	ST7789_CASET,
	4,
	// 5: Column addr set, 4 args, no delay:
	ST7789_XSTART >> 8,
	ST7789_XSTART & 0xff,
	//    XSTART = 0>>8, 0&0xff,
	(ST7789_WIDTH - 1) >> 8,
	(ST7789_WIDTH - 1) & 0xff,
	//    XEND = (320-1)>>8,(320-1)&0xff,
	ST7789_RASET,
	4,
	// 6: Row addr set, 4 args, no delay:
	ST7789_YSTART >> 8,
	ST7789_YSTART & 0xff,
	//    YSTART = 0>>8, 0&0xff,
	(ST7789_HEIGHT - 1) >> 8,
	(ST7789_HEIGHT - 1) & 0xff,
	//    YEND = (320-1)>>8,(320-1)&0xff,
    ST7789_INVON,
	DELAY,
	// 7: Inversion ON
	10,
	ST7789_NORON,
	DELAY,
	// 8: Normal display on, no args, w/delay
	10,
	// 10 ms delay
	ST7789_DISPON,
	DELAY,
	// 9: Main screen turn on, no args, w/delay
	10 
};
static void ST7789_WriteCommand(uint8_t cmd)
{
	ST7789_Select();
	ST7789_DC_Clr();
	HAL_SPI_Transmit(&ST7789_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
	ST7789_UnSelect();
}
/**
 * @brief Write data to ST7789 controller
 * @param buff -> pointer of data buffer
 * @param buff_size -> size of the data buffer
 * @return none
 */
static void ST7789_WriteData(uint8_t *buff, size_t buff_size)
{
	ST7789_Select();
	ST7789_DC_Set();

	// split data in small chunks because HAL can't send more than 64K at once

	while (buff_size > 0) {
		uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;
			if (DMA_MIN_SIZE <= buff_size)
			{
				HAL_SPI_Transmit_DMA(&ST7789_SPI_PORT, buff, chunk_size);
				while (ST7789_SPI_PORT.hdmatx->State != HAL_DMA_STATE_READY)
				{}
			}
			else
				HAL_SPI_Transmit(&ST7789_SPI_PORT, buff, chunk_size, HAL_MAX_DELAY);
		buff += chunk_size;
		buff_size -= chunk_size;
	}

	ST7789_UnSelect();
}
/**
 * @brief Write data to ST7789 controller, simplify for 8bit data.
 * data -> data to write
 * @return none
 */
static void ST7789_WriteSmallData(uint8_t data)
{
	ST7789_Select();
	ST7789_DC_Set();
	HAL_SPI_Transmit(&ST7789_SPI_PORT, &data, sizeof(data), HAL_MAX_DELAY);
	ST7789_UnSelect();
}
/**
 * @brief Set the rotation direction of the display
 * @param m -> rotation parameter(please refer it in st7789.h)
 * @return none
 */
void ST7789_SetRotation(uint8_t m)
{
	ST7789_WriteCommand(ST7789_MADCTL);	// MADCTL
	switch (m) {
	case 0:
		ST7789_WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
		x_shift = 0;
		y_shift = 80;
		break;
	case 1:
		ST7789_WriteSmallData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		x_shift = 80;
		y_shift = 0;
		break;
	case 2:
		ST7789_WriteSmallData(ST7789_MADCTL_RGB);
		x_shift = 0;
		y_shift = 0;
		break;
	case 3:
		ST7789_WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		x_shift = 0;
		y_shift = 0;
		break;
	default:
		break;
	}
}
/**
 * @brief Set address of DisplayWindow
 * @param xi&yi -> coordinates of window
 * @return none
 */
static void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	uint16_t x_start = x0 + x_shift, x_end = x1 + x_shift;
	uint16_t y_start = y0 + y_shift, y_end = y1 + y_shift;
	
	/* Column Address set */
	ST7789_WriteCommand(ST7789_CASET); 
	{
		uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
		ST7789_WriteData(data, sizeof(data));
	}

	/* Row Address set */
	ST7789_WriteCommand(ST7789_RASET);
	{
		uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
		ST7789_WriteData(data, sizeof(data));
	}
	/* Write to RAM */
	ST7789_WriteCommand(ST7789_RAMWR);
}
/**
 * @brief Initialize ST7789 controller
 * @param none
 * @return none
 */

static void ST7789_ExecuteCommandList(const uint8_t *addr)
{
	
	uint8_t numCommands, numArgs;
	uint16_t ms;

	numCommands = *addr++;
	while (numCommands--) {
		uint8_t cmd = *addr++;
		ST7789_WriteCommand(cmd);

		numArgs = *addr++;
		// If high bit set, delay follows args
		ms = numArgs & DELAY;
		numArgs &= ~DELAY;
		if (numArgs) {
			ST7789_WriteData((uint8_t*)addr, numArgs);
			addr += numArgs;
		}

		if (ms) {
			ms = *addr++;
			if (ms == 255) ms = 500;
			HAL_Delay(ms);
		}
	}
}

void ST7789_Init(void)
{
	HAL_Delay(300);
	ST7789_Select();
	
    ST7789_RST_Clr();
    HAL_Delay(10);
    ST7789_RST_Set();
    HAL_Delay(20);

	ST7789_ExecuteCommandList(init_cmds);
	ST7789_SetRotation(3);
	ST7789_UnSelect();
}

//driver
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//printing

//==============================================================================
// Процедура закрашивает экран цветом color
//==============================================================================
void ST7789_Fill_Color(uint16_t color)
{
	ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
	uint16_t i, j;
	for (i = 0; i < ST7789_HEIGHT; i++) {
		uint8_t data[ST7789_WIDTH * 2];
		for (j = 0; j < ST7789_WIDTH * 2; j += 2) {
			data[j] = color >> 8;
			data[j + 1] = color & 0xFF;
		}
		ST7789_WriteData(data, sizeof(data));
	}
}

//==============================================================================
// Процедура заполнения прямоугольника цветом color
//==============================================================================
void ST7789_FillRect(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
	if ((xEnd < 0) || (xEnd >= ST7789_WIDTH) ||
		 (yEnd < 0) || (yEnd >= ST7789_HEIGHT))	return;
	ST7789_SetAddressWindow(xSta, ySta, xEnd, yEnd);
	uint16_t i, j;
	
//	for (i = ySta; i <= yEnd; i++)
//		for (j = xSta; j <= xEnd; j++) {
//			uint8_t data[] = { color >> 8, color & 0xFF };
//			ST7789_WriteData(data, sizeof(data));
//		}
	
	for (i = ySta; i < yEnd + 1; i++) {
		uint8_t data[(xEnd - xSta + 1) * 2];
		for (j = 0; j < (xEnd - xSta + 1) * 2; j += 2) {
			data[j] = color >> 8;
			data[j + 1] = color & 0xFF;
		}
		ST7789_WriteData(data, sizeof(data));
	}
	
}

//==============================================================================
// Процедура окрашивает 1 пиксель дисплея
//==============================================================================
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	if ((x < 0) || (x >= ST7789_WIDTH) ||
		 (y < 0) || (y >= ST7789_HEIGHT))	return;
	
	ST7789_SetAddressWindow(x, y, x, y);
	uint8_t data[] = { color >> 8, color & 0xFF };
	ST7789_WriteData(data, sizeof(data));
}

//==============================================================================
// Процедура рисования символа ( 1 буква или знак )
//==============================================================================
void ST7789_WriteChar(uint16_t x, uint16_t y, uint8_t chShift, FontDef font, uint16_t color, uint16_t bgcolor)
{
	uint32_t i, b, j;

	ST7789_SetAddressWindow(x, y, x + font.width - 1, y + font.height - 1);
	uint8_t chArr[font.width * 2];
	for (i = 0; i < font.height; i++)
	{
		b = font.data[chShift * font.height + i];
		for (j = 0; j < font.width; j++)
		{
			if ((b << j) & 0x8000)
			{
				chArr[j * 2] = color >> 8;
				chArr[j * 2 + 1] = color & 0xFF;
			}
			else
			{
				chArr[j * 2] = bgcolor >> 8;
				chArr[j * 2 + 1] = bgcolor & 0xFF;
			}
		}
		ST7789_WriteData(chArr, sizeof(chArr));
	}
}

//==============================================================================
// Процедура рисования символа ( 1 буква или знак ) (в 2 раза больше)
//==============================================================================
void ST7789_WriteChar_x2(uint16_t x, uint16_t y, uint8_t chShift, FontDef font, uint16_t color, uint16_t bgcolor)
{
	uint32_t i, b, j;

	ST7789_SetAddressWindow(x, y, x + font.width * 2 - 1, y + font.height * 2 - 1);
	uint8_t chArr[font.width * 2 * 4];
	for (i = 0; i < font.height; i++)
	{
		b = font.data[chShift * font.height + i];
		for (j = 0; j < font.width; j++)
		{
			if ((b << j) & 0x8000)
			{
				chArr[j * 4] = color >> 8;
				chArr[j * 4 + 1] = color & 0xFF;
				chArr[j * 4 + 2] = color >> 8;
				chArr[j * 4 + 3] = color & 0xFF;
				
				chArr[j * 4 + font.width * 4] = color >> 8;
				chArr[j * 4 + 1 + font.width * 4] = color & 0xFF;
				chArr[j * 4 + 2 + font.width * 4] = color >> 8;
				chArr[j * 4 + 3 + font.width * 4] = color & 0xFF;
			}
			else
			{
				chArr[j * 4] = bgcolor >> 8;
				chArr[j * 4 + 1] = bgcolor & 0xFF;
				chArr[j * 4 + 2] = bgcolor >> 8;
				chArr[j * 4 + 3] = bgcolor & 0xFF;
				
				chArr[j * 4 + font.width * 4] = bgcolor >> 8;
				chArr[j * 4 + 1 + font.width * 4] = bgcolor & 0xFF;
				chArr[j * 4 + 2 + font.width * 4] = bgcolor >> 8;
				chArr[j * 4 + 3 + font.width * 4] = bgcolor & 0xFF;
			}
		}
		ST7789_WriteData(chArr, sizeof(chArr));
	}
}

//==============================================================================
// Процедура рисования строки
//==============================================================================
void ST7789_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
	while (*str) {
		if (x + font.width >= ST7789_WIDTH) {
			x = 0;
			y += font.height;
			if (y + font.height >= ST7789_HEIGHT) {
				break;
			}

			if (*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		uint8_t chShift = *str - 32;
		
		if (*str == 0xD0 || *str == 0xD1) {
			switch(*str) {
			case 0xD0: 
				{
					str++;                                                 // увеличиваем указатель так как во втором байте сам символ
					if ((uint8_t)*str >= 0x90 && (uint8_t)*str <= 0xBF)  // байт символов А...Я а...п 
						chShift = (*str) - 0x90 + 96;
					else if ((uint8_t)*str == 0x81) {  // байт символа Ё ( если нужнф еще символы добавляем тут и в функции DrawChar() )
						chShift = 160;
					}		
					break;
				}
			case 0xD1:
				{
					str++;                                                  // увеличиваем указатель так как во втором байте сам символ
					if ((uint8_t)*str >= 0x80 && (uint8_t)*str <= 0x8F)
						chShift = (*str) - 0x80 + 144;                      // байт символов п...я 
					else if ((uint8_t)*str == 0x91) {  						// байт символа ё ( если нужнф еще символы добавляем тут и в функции DrawChar() )
						chShift = 161; 
					}		
					break;
				}
			}
		}
		ST7789_WriteChar(x, y, chShift, font, color, bgcolor);
		x += font.width;
		str++;
	}
}

//==============================================================================
// Процедура рисования строки (в 2 раза больше)
//==============================================================================
void ST7789_WriteString_x2(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
	while (*str) {
		if (x + font.width * 2 >= ST7789_WIDTH) {
			x = 0;
			y += font.height * 2;
			if (y + font.height * 2 >= ST7789_HEIGHT) {
				break;
			}

			if (*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		uint8_t chShift = *str - 32;
		
		if (*str == 0xD0 || *str == 0xD1) {
			switch (*str) {
			case 0xD0: 
				{
					str++; // увеличиваем указатель так как во втором байте сам символ
					if ((uint8_t)*str >= 0x90 && (uint8_t)*str <= 0xBF)  // байт символов А...Я а...п 
						chShift = (*str) - 0x90 + 96;
					else if ((uint8_t)*str == 0x81) {
						// байт символа Ё ( если нужнф еще символы добавляем тут и в функции DrawChar() )
						chShift = 160;
					}		
					break;
				}
			case 0xD1:
				{
					str++; // увеличиваем указатель так как во втором байте сам символ
					if ((uint8_t)*str >= 0x80 && (uint8_t)*str <= 0x8F)
						chShift = (*str) - 0x80 + 144; // байт символов п...я 
					else if ((uint8_t)*str == 0x91) {
						// байт символа ё ( если нужнф еще символы добавляем тут и в функции DrawChar() )
						chShift = 161; 
					}		
					break;
				}
			}
		}
		ST7789_WriteChar_x2(x, y, chShift, font, color, bgcolor);
		x += font.width * 2;
		str++;
	}
}
