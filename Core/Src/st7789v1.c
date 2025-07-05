/*

  ******************************************************************************
  * @file 			( фаил ):   ST7789.c
  * @brief 		( описание ):  	
  ******************************************************************************
  * @attention 	( внимание ):	 author: Golinskiy Konstantin	e-mail: golinskiy.konstantin@gmail.com
  ******************************************************************************
  
*/

#include "ST7789v1.h"


uint16_t ST7789_X_Start = ST7789_XSTART;	
uint16_t ST7789_Y_Start = ST7789_YSTART;

uint16_t ST7789_Width = 0;
uint16_t ST7789_Height = 0;

#if FRAME_BUFFER
// массив буфер кадра
	uint16_t buff_frame[ST7789_WIDTH*ST7789_HEIGHT] = { 0x0000, };
#endif

static void ST7789_ExecuteCommandList(const uint8_t *addr);
static void ST7789_Unselect(void);
static void ST7789_Select(void);
static void ST7789_SendCmd(uint8_t Cmd);
static void ST7789_SendData(uint8_t Data );
static void ST7789_SendDataMASS(uint8_t* buff, size_t buff_size);
static void ST7789_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void ST7789_ColumnSet(uint16_t ColumnStart, uint16_t ColumnEnd);
static void ST7789_RowSet(uint16_t RowStart, uint16_t RowEnd);
static void SwapInt16Values(int16_t *pValue1, int16_t *pValue2);
static void ST7789_DrawLine_Slow(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void ST7789_RamWrite(uint16_t *pBuff, uint32_t Len);


//==== данные для инициализации дисплея ST7789_240X320 ==========

// инициализация для всех дисплеев одна, так как драйвер расчитан на максимальный размер 240x320
// для подгона пор свой размер двигаем в функции ротации дисплея
static const uint8_t init_cmds[] = {
		9,                       			// 9 commands in list:
		ST7789_SWRESET,   DELAY,    		// 1: Software reset, no args, w/delay
		  150,                     			//    150 ms delay
		ST7789_SLPOUT ,  DELAY,    			// 2: Out of sleep mode, no args, w/delay
		  255,                            	//    255 = 500 ms delay
		ST7789_COLMOD , 1+DELAY,    		// 3: Set color mode, 1 arg + delay:
		  (ST7789_ColorMode_65K | ST7789_ColorMode_16bit),           //    16-bit color 0x55
		  10,                             	//    10 ms delay
		ST7789_MADCTL , 1,                 	// 4: Memory access ctrl (directions), 1 arg:
		  ST7789_ROTATION,                  //    Row addr/col addr, bottom to top refresh
		ST7789_CASET  , 4,                 	// 5: Column addr set, 4 args, no delay:
		  ST7789_XSTART>>8,ST7789_XSTART&0xff,  //    XSTART = 0>>8, 0&0xff,
		  (ST7789_WIDTH-1)>>8,(ST7789_WIDTH-1)&0xff,    //    XEND = (320-1)>>8,(320-1)&0xff,
		ST7789_RASET  , 4,                 	// 6: Row addr set, 4 args, no delay:
		  ST7789_YSTART>>8,ST7789_YSTART&0xff,  //    YSTART = 0>>8, 0&0xff,
		  (ST7789_HEIGHT-1)>>8,(ST7789_HEIGHT-1)&0xff,  //    YEND = (320-1)>>8,(320-1)&0xff,
		ST7789_INVON ,   DELAY,     		// 7: Inversion ON
		  10,
		ST7789_NORON  ,   DELAY,    		// 8: Normal display on, no args, w/delay
		  10,                              	// 10 ms delay
		ST7789_DISPON ,   DELAY,    		// 9: Main screen turn on, no args, w/delay
		  10 
	};
	//---------------------------------------------------------------------------------------------
	
//===============================================================
	
	
//##############################################################################
	  
	  
//==============================================================================
	  
	  
	  
//==============================================================================
// Процедура инициализации дисплея
//==============================================================================
void ST7789_Init(void){
	
	// Задержка после подачи питания
	// если при старте не всегда запускаеться дисплей увеличиваем время задержки
	HAL_Delay(200);	
	
	ST7789_Width = ST7789_WIDTH;
	ST7789_Height = ST7789_HEIGHT;
	
  ST7789_Select();

  ST7789_HardReset(); 
  ST7789_ExecuteCommandList(init_cmds);
	
  ST7789_Unselect();
	
#if FRAME_BUFFER
	ST7789_ClearFrameBuffer();
#endif

}
//==============================================================================


//==============================================================================
// Процедура управления SPI
//==============================================================================
static void ST7789_Select(void) {
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
}
//==============================================================================


//==============================================================================
// Процедура управления SPI
//==============================================================================
static void ST7789_Unselect(void) {
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}


//==============================================================================
// Процедура отправки данных для инициализации дисплея
//==============================================================================
static void ST7789_ExecuteCommandList(const uint8_t *addr) {
	
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--) {
        uint8_t cmd = *addr++;
        ST7789_SendCmd(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
            ST7789_SendDataMASS((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            HAL_Delay(ms);
        }
    }
}



//==============================================================================
// Процедура аппаратного сброса дисплея (ножкой RESET)
//==============================================================================
void ST7789_HardReset(void){

	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(20);	
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
	
}
//==============================================================================


//==============================================================================
// Процедура отправки команды в дисплей
//==============================================================================
__inline static void ST7789_SendCmd(uint8_t Cmd){	
	
		 // pin DC LOW
		 HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
					 
		 HAL_SPI_Transmit(&ST7789_SPI_HAL, &Cmd, 1, HAL_MAX_DELAY);
		 while(HAL_SPI_GetState(&ST7789_SPI_HAL) != HAL_SPI_STATE_READY){};
				
		 // pin DC HIGH
		 HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
		 

}
//==============================================================================


//==============================================================================
// Процедура отправки данных (параметров) в дисплей 1 BYTE
//==============================================================================
__inline static void ST7789_SendData(uint8_t Data ){
	
		HAL_SPI_Transmit(&ST7789_SPI_HAL, &Data, 1, HAL_MAX_DELAY);
		while(HAL_SPI_GetState(&ST7789_SPI_HAL) != HAL_SPI_STATE_READY){};
}



//==============================================================================
// Процедура отправки данных (параметров) в дисплей MASS
//==============================================================================
__inline static void ST7789_SendDataMASS(uint8_t* buff, size_t buff_size){
		
		if( buff_size <= 0xFFFF ){
			HAL_SPI_Transmit(&ST7789_SPI_HAL, buff, buff_size, HAL_MAX_DELAY);
		}
		else{
			while( buff_size > 0xFFFF ){
				HAL_SPI_Transmit(&ST7789_SPI_HAL, buff, 0xFFFF, HAL_MAX_DELAY);
				buff_size-=0xFFFF;
				buff+=0xFFFF;
			}
			HAL_SPI_Transmit(&ST7789_SPI_HAL, buff, buff_size, HAL_MAX_DELAY);
		}
}
//==============================================================================


//==============================================================================
// Процедура включения режима сна
//==============================================================================
void ST7789_SleepModeEnter( void ){
	
	ST7789_Select(); 
	
	ST7789_SendCmd(ST7789_SLPIN);
	
	ST7789_Unselect();
	
	HAL_Delay(250);
}
//==============================================================================


//==============================================================================
// Процедура отключения режима сна
//==============================================================================
void ST7789_SleepModeExit( void ){
	
	ST7789_Select(); 
	
	ST7789_SendCmd(ST7789_SLPOUT);
	
	ST7789_Unselect();
	
	HAL_Delay(250);
}
//==============================================================================



//==============================================================================
// Процедура закрашивает экран цветом color
//==============================================================================
void ST7789_FillScreen(uint16_t color){
	
  ST7789_FillRect(0, 0,  ST7789_Width, ST7789_Height, color);
}
//==============================================================================



//==============================================================================
// Процедура заполнения прямоугольника цветом color
//==============================================================================
void ST7789_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){
	
  if ((x >= ST7789_Width) || (y >= ST7789_Height)){
	  return;
  }
  
  if ((x + w) > ST7789_Width){	  
	  w = ST7789_Width - x;
  }
  
  if ((y + h) > ST7789_Height){
	  h = ST7789_Height - y;
  }
  ST7789_SetWindow(x, y, x + w - 1, y + h - 1);
		
  ST7789_RamWrite(&color, (h * w));
}
//==============================================================================


//==============================================================================
// Процедура установка границ экрана для заполнения
//==============================================================================
static void ST7789_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

	ST7789_Select();

	ST7789_ColumnSet(x0, x1);
	ST7789_RowSet(y0, y1);

	// write to RAM
	ST7789_SendCmd(ST7789_RAMWR);

	ST7789_Unselect();

}
//==============================================================================


//==============================================================================
// Процедура записи данных в дисплей
//==============================================================================
	void ST7789_RamWrite(uint16_t *pBuff, uint32_t Len) {
	
		ST7789_Select();
	
		uint8_t buff[2];
		buff[0] = *pBuff >> 8;
		buff[1] = *pBuff & 0xFF;
	
		while (Len--) {
			ST7789_SendDataMASS(buff, 2);
		} 
	
		ST7789_Unselect();
	}
//==============================================================================


//==============================================================================
// Процедура установки начального и конечного адресов колонок
//==============================================================================
void ST7789_ColumnSet(uint16_t ColumnStart, uint16_t ColumnEnd){
	
  if (ColumnStart > ColumnEnd){
    return;
  }
  
  if (ColumnEnd > ST7789_Width){
    return;
  }
  
  ColumnStart += ST7789_X_Start;
  ColumnEnd += ST7789_X_Start;
  
  ST7789_SendCmd(ST7789_CASET);
  ST7789_SendData(ColumnStart >> 8);  
  ST7789_SendData(ColumnStart & 0xFF);  
  ST7789_SendData(ColumnEnd >> 8);  
  ST7789_SendData(ColumnEnd & 0xFF);  
  
}
//==============================================================================


//==============================================================================
// Процедура установки начального и конечного адресов строк
//==============================================================================
static void ST7789_RowSet(uint16_t RowStart, uint16_t RowEnd){
	
  if (RowStart > RowEnd){
    return;
  }
  
  if (RowEnd > ST7789_Height){
    return;
  }
  
  RowStart += ST7789_Y_Start;
  RowEnd += ST7789_Y_Start;
 
  ST7789_SendCmd(ST7789_RASET);
  ST7789_SendData(RowStart >> 8);  
  ST7789_SendData(RowStart & 0xFF);  
  ST7789_SendData(RowEnd >> 8);  
  ST7789_SendData(RowEnd & 0xFF);  

}


//==============================================================================
// Процедура ротации ( положение ) дисплея
//==============================================================================
// па умолчанию 1 режим ( всего 1, 2, 3, 4 )
void ST7789_rotation( uint8_t rotation ){
	
	ST7789_Select();
	
	ST7789_SendCmd(ST7789_MADCTL);

	// длайвер расчитан на экран 320 х 240 (  максимальный размер )
	// для подгона под любой другой нужно отнимать разницу пикселей

	  switch (rotation) {
		
		case 1:
			//== 1.13" 135 x 240 ST7789 =================================================
			#ifdef ST7789_IS_135X240
				ST7789_SendData(ST7789_MADCTL_RGB);
				ST7789_Width = 135;
				ST7789_Height = 240;
				ST7789_X_Start = 52;
				ST7789_Y_Start = 40;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
		
			//== 1.3" 240 x 240 ST7789 =================================================
			#ifdef ST7789_IS_240X240
				ST7789_SendData(ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 240;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
			
			//== 1.47" 172 x 320 ST7789 =================================================
			#ifdef ST7789_IS_172X320
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 320;
				ST7789_Height = 172;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 34;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
			
			//== 1.69" 240 x 280 ST7789 =================================================
			#ifdef ST7789_IS_240X280
				ST7789_SendData(ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 280;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 20;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
			
			//== 2" 240 x 320 ST7789 =================================================
			#ifdef ST7789_IS_240X320
				ST7789_SendData(ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 320;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
		 break;
		
		case 2:
			//== 1.13" 135 x 240 ST7789 =================================================
			#ifdef ST7789_IS_135X240
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 135;
				ST7789_X_Start = 40;
				ST7789_Y_Start = 53;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
		
			//== 1.3" 240 x 240 ST7789 =================================================
			#ifdef ST7789_IS_240X240
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 240;		
				ST7789_X_Start = 0;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
			
			//== 1.47" 172 x 320 ST7789 =================================================
			#ifdef ST7789_IS_172X320
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
				ST7789_Width = 172;
				ST7789_Height = 320;
				ST7789_X_Start = 34;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
			
			//== 1.69" 240 x 280 ST7789 =================================================
			#ifdef ST7789_IS_240X280
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 280;
				ST7789_Height = 240;
				ST7789_X_Start = 20;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
			
			//== 2" 240 x 320 ST7789 =================================================
			#ifdef ST7789_IS_240X320
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 320;
				ST7789_Height = 240;		
				ST7789_X_Start = 0;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
		 break;
		
	   case 3:
		   //== 1.13" 135 x 240 ST7789 =================================================
			#ifdef ST7789_IS_135X240
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
				ST7789_Width = 135;
				ST7789_Height = 240;
				ST7789_X_Start = 53;
				ST7789_Y_Start = 40;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
	   
			//== 1.3" 240 x 240 ST7789 =================================================
			#ifdef ST7789_IS_240X240
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 240;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 80;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
	   
			//== 1.47" 172 x 320 ST7789 =================================================
			#ifdef ST7789_IS_172X320
				ST7789_SendData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 320;
				ST7789_Height = 172;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 34;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
			
			//== 1.69" 240 x 280 ST7789 =================================================
			#ifdef ST7789_IS_240X280
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 280;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 20;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
			
			//== 2" 240 x 320 ST7789 =================================================
			#ifdef ST7789_IS_240X320
				ST7789_SendData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 320;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
			
		 break;
	   
	   case 4:
		   //== 1.13" 135 x 240 ST7789 =================================================
			#ifdef ST7789_IS_135X240
				ST7789_SendData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 135;
				ST7789_X_Start = 40;
				ST7789_Y_Start = 52;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
	   
			//== 1.3" 240 x 240 ST7789 =================================================
			#ifdef ST7789_IS_240X240
				ST7789_SendData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 240;
				ST7789_Height = 240;
				ST7789_X_Start = 80;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
	   
		  //== 1.47" 172 x 320 ST7789 =================================================
			#ifdef ST7789_IS_172X320
				ST7789_SendData(ST7789_MADCTL_RGB);
				ST7789_Width = 172;
				ST7789_Height = 320;
				ST7789_X_Start = 34;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
			
			//== 1.69" 240 x 280 ST7789 =================================================
			#ifdef ST7789_IS_240X280
				ST7789_SendData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 280;
				ST7789_Height = 240;
				ST7789_X_Start = 20;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
			
			//== 2" 240 x 320 ST7789 =================================================
			#ifdef ST7789_IS_240X320
				ST7789_SendData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
				ST7789_Width = 320;
				ST7789_Height = 240;
				ST7789_X_Start = 0;
				ST7789_Y_Start = 0;
				ST7789_FillScreen(0);
			#endif
			//==========================================================================
		 break;
	   
	   default:
		 break;
	  }
	  
	  ST7789_Unselect();
}
