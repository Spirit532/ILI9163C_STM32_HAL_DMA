

# ILI9163C STM32 HAL DMA driver
A simple, very fast ILI9163C display driver written in C for STM32, using HAL, DMA, and a full frame buffer.
## Pros

- Blazing fast - with SPI running at 41.5MHz, this driver can update the display running in 16-bit color mode at over **80 frames per second**, with little tearing to be seen.
- Low amount of processing power required on the instruction end - all the graphical functions write to a frame buffer, eliminating constant polled SPI transactions.
- It *works*. I've encountered many issues while trying to run several ILI9163C displays(which are technically compatible with the ST7735), but as far as I can tell, this driver will run just about all of them in landscape and inverted landscape modes.

## Cons

- The memory footprint. Since this driver uses a full frame buffer, it immediately consumes around ~**50kB(!)** of RAM, 160x128x2 bytes for 16-bit color + some overhead. In the application this was developed for(STM32G474), this is an acceptable sacrifice. However, smaller devices simply don't have this amount of RAM - let alone enough to justify using this driver.
To eliminate this, advanced techniques may be used, but this driver applies the brute force technique.
- Code quality - the driver is in a relatively raw state, and while fully functional and relatively optimized, I didn't put a serious amount of time into it, so anyone who wonders inside, prepare for horrors.

## Usage
SPI DMA must be set up in circular mode, single byte data width, memory to peripheral. Ensure that all the display pins are set to the highest drive strength possible on your chip. At higher SPI clock rates, it won't work if they're not.

Include the required files, then declare the following variables somewhere:
```C
uint8_t SPI_DMA_FL = 0;
uint32_t SPI_DMA_CNT=1;
```
After that, set up a DMA Transmission Complete interrupt callback, and put this in there:
```C
SPI_DMA_CNT--;
if(SPI_DMA_CNT==0)
{
	HAL_SPI_DMAStop(&hspi1);
	SPI_DMA_CNT=1;
	SPI_DMA_FL=1;
}
```

Then, initialize SPI, followed by the display:
```C
MX_SPIx_Init();
ILI9163_init(1); // 0 = normal landscape, 1 = rotated 180 degrees
```
In your display handling function(e.g. RTOS thread), begin a new frame by emptying the frame buffer:
```C
ILI9163_newFrame();
```
Then, call the necessary graphical functions:
```C
ILI9163_drawString(5, 5, Font_7x10, BLACK, "test");
ILI9163_fillRect(40, 60, 50, 70, ORANGE);
ILI9163_drawRect(50, 50, 75, 75, 3, RED);

uint16_t test = 111;
ILI9163_drawStringF(10, 10, Font_7x10, BLACK, "%u", test);
```
And finally, tell the driver to send the buffer to the display(this is a blocking function!):
```C
ILI9163_render();
```

## Stuff
This library is unmaintained and is licensed under the MIT license. Pull requests are welcome.
If you see some of your code in this library, let me know and I will credit you.
