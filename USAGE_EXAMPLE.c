#include "main.h"

#include "ili9163.h"

uint8_t SPI_DMA_FL = 0;
uint32_t SPI_DMA_CNT=1;

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) // Your TxCpltCallback
{
	SPI_DMA_CNT--;
	if(SPI_DMA_CNT==0)
	{
		HAL_SPI_DMAStop(&hspi1);
		SPI_DMA_CNT=1;
		SPI_DMA_FL=1;
	}
}

yourMainFunction
{
	MX_DMA_Init();
	MX_SPI1_Init();
	
	ILI9163_init(1); // Flipped landscape
}

yourRenderFunction
{
	ILI9163_newFrame(); // Call this to clear the frame buffer out
	
	ILI9163_drawString(5, 5, Font_7x10, BLACK, "test");
	ILI9163_fillRect(40, 60, 50, 70, ORANGE);
	ILI9163_drawRect(50, 50, 75, 75, 3, RED);
	
	uint16_t test = 111;
	ILI9163_drawStringF(10, 10, Font_7x10, BLACK, "%u", test); // The graphical functions don't cost a lot
	
	ILI9163_render(); // This draws the whole frame buffer via DMA at once
}