#include "ili9163.h"
#include <stdio.h> // printf
#include <stdarg.h> // va_list, va_start, va_arg, va_end

uint8_t frameBuffer[BUFSIZE] = {0};

extern SPI_HandleTypeDef DISP_SPI;
extern uint8_t SPI_DMA_FL;

void ILI9163_writeCommand(uint8_t address) {
	HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, 0);
	HAL_GPIO_WritePin(DISP_DC_Port, DISP_DC_Pin, 0);

	HAL_SPI_Transmit(&DISP_SPI, &address, 1, 0);

	HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, 1);
}

void ILI9163_writeData(uint8_t data) {
	HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, 0);
	HAL_GPIO_WritePin(DISP_DC_Port, DISP_DC_Pin, 1);

	HAL_SPI_Transmit(&DISP_SPI, &data, 1, 0);

	HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, 1);
}

void ILI9163_writeData16(uint16_t word) {
	HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, 0);
	HAL_GPIO_WritePin(DISP_DC_Port, DISP_DC_Pin, 1);

	uint8_t data [2] = {(word >> 8) & 0x00FF, word & 0x00FF};
	HAL_SPI_Transmit(&DISP_SPI, data, 2, 0);

	HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, 1);
}

void ILI9163_setAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2) {
	ILI9163_writeCommand(ILI9163_CMD_SET_COLUMN_ADDRESS);
	ILI9163_writeData16(x1);
	ILI9163_writeData16(x2);

	ILI9163_writeCommand(ILI9163_CMD_SET_PAGE_ADDRESS);
	ILI9163_writeData16(y1);
	ILI9163_writeData16(y2);

	ILI9163_writeCommand(ILI9163_CMD_WRITE_MEMORY_START);
}

void ILI9163_reset(void)
{
	HAL_GPIO_WritePin(DISP_RST_Port, DISP_RST_Pin, 0);
	HAL_Delay(10);

	HAL_GPIO_WritePin(DISP_RST_Port, DISP_RST_Pin, 1);
	HAL_Delay(100);
}

void ILI9163_init(int rotation) {
	HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, 1);
	HAL_GPIO_WritePin(DISP_RST_Port, DISP_RST_Pin, 1);

	ILI9163_reset(); // Hardware reset the LCD

	ILI9163_writeCommand(ILI9163_CMD_EXIT_SLEEP_MODE);
	HAL_Delay(5); // Wait for the screen to wake up

	ILI9163_writeCommand(ILI9163_CMD_SET_PIXEL_FORMAT);
	ILI9163_writeData(0x05); // 16 bpp

	ILI9163_writeCommand(ILI9163_CMD_SET_GAMMA_CURVE);
	ILI9163_writeData(0x04); // Gamma curve 3

	ILI9163_writeCommand(ILI9163_CMD_GAM_R_SEL);
	ILI9163_writeData(0x01); // Gamma curve enable

	ILI9163_writeCommand(ILI9163_CMD_POSITIVE_GAMMA_CORRECT);
	ILI9163_writeData(0x3f);
	ILI9163_writeData(0x25);
	ILI9163_writeData(0x1c);
	ILI9163_writeData(0x1e);
	ILI9163_writeData(0x20);
	ILI9163_writeData(0x12);
	ILI9163_writeData(0x2a);
	ILI9163_writeData(0x90);
	ILI9163_writeData(0x24);
	ILI9163_writeData(0x11);
	ILI9163_writeData(0x00);
	ILI9163_writeData(0x00);
	ILI9163_writeData(0x00);
	ILI9163_writeData(0x00);
	ILI9163_writeData(0x00);

	ILI9163_writeCommand(ILI9163_CMD_NEGATIVE_GAMMA_CORRECT);
	ILI9163_writeData(0x20);
	ILI9163_writeData(0x20);
	ILI9163_writeData(0x20);
	ILI9163_writeData(0x20);
	ILI9163_writeData(0x05);
	ILI9163_writeData(0x00);
	ILI9163_writeData(0x15);
	ILI9163_writeData(0xa7);
	ILI9163_writeData(0x3d);
	ILI9163_writeData(0x18);
	ILI9163_writeData(0x25);
	ILI9163_writeData(0x2a);
	ILI9163_writeData(0x2b);
	ILI9163_writeData(0x2b);
	ILI9163_writeData(0x3a);

	ILI9163_writeCommand(ILI9163_CMD_FRAME_RATE_CONTROL1);
	ILI9163_writeData(0x08); // DIVA = 8
	ILI9163_writeData(0x02); // VPA = 8

	ILI9163_writeCommand(ILI9163_CMD_FRAME_RATE_CONTROL2);
	ILI9163_writeData(0x08); // DIVA = 8
	ILI9163_writeData(0x02); // VPA = 8

	ILI9163_writeCommand(ILI9163_CMD_FRAME_RATE_CONTROL3);
	ILI9163_writeData(0x08); // DIVA = 8
	ILI9163_writeData(0x02); // VPA = 8

	ILI9163_writeCommand(ILI9163_CMD_DISPLAY_INVERSION);
	ILI9163_writeData(0x07); // NLA = 1, NLB = 1, NLC = 1 (all on Frame Inversion)

	ILI9163_writeCommand(ILI9163_CMD_POWER_CONTROL1);
	ILI9163_writeData(0x0a); // VRH = 10:  GVDD = 4.30
	ILI9163_writeData(0x02); // VC = 2: VCI1 = 2.65

	ILI9163_writeCommand(ILI9163_CMD_POWER_CONTROL2);
	ILI9163_writeData(0x02); // BT = 2: AVDD = 2xVCI1, VCL = -1xVCI1, VGH = 5xVCI1, VGL = -2xVCI1

	ILI9163_writeCommand(ILI9163_CMD_VCOM_CONTROL1);
	ILI9163_writeData(0x24); // VMH = 80: VCOMH voltage = 4.5
	ILI9163_writeData(0x48); // VML = 91: VCOML voltage = -0.225

	ILI9163_writeCommand(ILI9163_CMD_VCOM_OFFSET_CONTROL);
	ILI9163_writeData(0x40); // nVM = 0, VMF = 64: VCOMH output = VMH, VCOML output = VML

	ILI9163_writeCommand(ILI9163_CMD_SET_COLUMN_ADDRESS);
	ILI9163_writeData(0x00); // XSH
	ILI9163_writeData(0x00); // XSL
	ILI9163_writeData(0x00); // XEH
	ILI9163_writeData(ILI9163_HEIGHT-1); // XEL (128 pixels x)

	ILI9163_writeCommand(ILI9163_CMD_SET_PAGE_ADDRESS);
	ILI9163_writeData(0x00);
	ILI9163_writeData(0x00);
	ILI9163_writeData(0x00);
	ILI9163_writeData(ILI9163_WIDTH-1); // 160 pixels y

	ILI9163_writeCommand(ILI9163_CMD_SET_ADDRESS_MODE);
	if(rotation)
		ILI9163_writeData(160);
	else
		ILI9163_writeData(96);

	ILI9163_writeCommand(ILI9163_CMD_SET_DISPLAY_ON);
	ILI9163_writeCommand(ILI9163_CMD_WRITE_MEMORY_START);
}

void ILI9163_newFrame()
{
	for(uint32_t i= 0; i < (ILI9163_WIDTH*ILI9163_HEIGHT)*2; i++)
		frameBuffer[i] = 0xFF;
}

void ILI9163_render()
{
	ILI9163_setAddress(0, 0, ILI9163_WIDTH, ILI9163_HEIGHT);
	HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, 0);
	HAL_GPIO_WritePin(DISP_DC_Port, DISP_DC_Pin, 1);

	HAL_SPI_Transmit_DMA(&DISP_SPI, frameBuffer, BUFSIZE);
	while(!SPI_DMA_FL) {}

	HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, 1);
	SPI_DMA_FL=0;
}

void ILI9163_drawPixel(uint16_t x, uint16_t y, uint16_t color) {
	if ((x < 0) || (x >= ILI9163_WIDTH) || (y < 0) || (y >= ILI9163_HEIGHT)) return;
	frameBuffer[((x*2)+(y*2*ILI9163_WIDTH))] = color >> 8;
	frameBuffer[((x*2)+(y*2*ILI9163_WIDTH))+1] = color & 0xFF;
}

void ILI9163_fillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	for(uint16_t x = x1; x < x2; x++)
		for(uint16_t y = y1; y < y2; y++)
			ILI9163_drawPixel(x, y, color);
}

void ILI9163_drawRect(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2, uint16_t thickness, uint16_t color) {
	ILI9163_fillRect(x1, y1, x2, y1+thickness, color);
	ILI9163_fillRect(x1, y2-thickness, x2, y2, color);

	ILI9163_fillRect(x1, y1, x1+thickness, y2, color);
	ILI9163_fillRect(x2-thickness, y1, x2, y2, color);
}

void ILI9163_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	uint16_t dy = y1 - y0;
	uint16_t dx = x1 - x0;
	uint16_t stepx, stepy;

	if (dy < 0) {
		dy = -dy; stepy = -1;
	}
	else stepy = 1;

	if (dx < 0) {
		dx = -dx; stepx = -1;
	}
	else stepx = 1;

	dy <<= 1;
	dx <<= 1;

	ILI9163_drawPixel(x0, y0, color);

	if (dx > dy) {
		int fraction = dy - (dx >> 1);
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;
			}

			x0 += stepx;
			fraction += dy;
			ILI9163_drawPixel(x0, y0, color);
		}
	} else {
		int fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}

			y0 += stepy;
			fraction += dx;
			ILI9163_drawPixel(x0, y0, color);
		}
	}
}


void ILI9163_fillCircle(uint16_t centerX, uint16_t centerY, uint16_t radius, uint16_t color) {
	for(int y=-radius; y<=radius; y++)
		for(int x=-radius; x<=radius; x++)
			if(x*x+y*y <= radius*radius)
				ILI9163_drawPixel(centerX+x, centerY+y, color);
}

void ILI9163_drawCircle(uint16_t centerX, uint16_t centerY, uint16_t radius, uint16_t color) { // From the Adafruit GFX library
	radius--; // inner outline
	int16_t f = 1 - radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * radius;
	int16_t x = 0;
	int16_t y = radius;

	ILI9163_drawPixel(centerX, centerY + radius, color);
	ILI9163_drawPixel(centerX, centerY - radius, color);
	ILI9163_drawPixel(centerX + radius, centerY, color);
	ILI9163_drawPixel(centerX - radius, centerY, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ILI9163_drawPixel(centerX + x, centerY + y, color);
		ILI9163_drawPixel(centerX - x, centerY + y, color);
		ILI9163_drawPixel(centerX + x, centerY - y, color);
		ILI9163_drawPixel(centerX - x, centerY - y, color);
		ILI9163_drawPixel(centerX + y, centerY + x, color);
		ILI9163_drawPixel(centerX - y, centerY + x, color);
		ILI9163_drawPixel(centerX + y, centerY - x, color);
		ILI9163_drawPixel(centerX - y, centerY - x, color);
	}
}

void ILI9163_fillDisplay(uint16_t color) {
	ILI9163_fillRect(0,0, ILI9163_WIDTH, ILI9163_HEIGHT, color);
}

void ILI9163_drawChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color) {
	uint32_t i, b, j;
	for(i = 0; i < font.height; i++) {
		b = font.data[(ch - 32) * font.height + i];
		for(j = 0; j < font.width; j++) {
			if((b << j) & 0x8000)  {
				ILI9163_drawPixel(x + j, y + i, color);
			}
		}
	}
}

void ILI9163_drawString(uint16_t x, uint16_t y, FontDef font, uint16_t color, const char *string) {
	while(*string) {
		if(x + font.width >= ILI9163_WIDTH) {
			x = 0;
			y += font.height;
			if(y + font.height >= ILI9163_HEIGHT)
				break;

			if(*string == ' ') {
				// skip spaces in the beginning of the new line
				string++;
				continue;
			}
		}

		ILI9163_drawChar(x, y, *string, font, color);

		x += font.width;
		string++;
	}
}

void ILI9163_drawStringF(uint16_t x, uint16_t y, FontDef font, uint16_t color, char *szFormat, ...) {
	char szBuffer[64];
	va_list pArgs;
	va_start(pArgs, szFormat);
	vsnprintf(szBuffer, 63, szFormat, pArgs);
	va_end(pArgs);

	ILI9163_drawString(x, y, font, color, szBuffer);
}
