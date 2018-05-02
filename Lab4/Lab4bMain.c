#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "library.h"
#include "graphics.h"

// Functions to be implemented in assembly ...
extern uint8_t *	BitmapAddress(char ascii, uint8_t *fontTable, int charHeight, int charWidth) ;
extern uint32_t		GetBitmapRow(uint8_t *prow) ;
extern uint32_t *	PixelAddress(int x, int y) ;

typedef struct
	{
	const uint8_t *table ;
	const uint16_t Width ;
	const uint16_t Height ;
	} sFONT;

extern sFONT Font8  ;
extern sFONT Font12 ;
extern sFONT Font16 ;
extern sFONT Font20 ;
extern sFONT Font24 ;

#define	X_MIN	0
#define	Y_MIN	40

#define	WIDTH	XPIXELS
#define	HEIGHT	(YPIXELS-Y_MIN-15)

static void		BarnsleyFernFractal(void) ;
static void		MandelbrotSetFractal(void) ;
static void		JuliaSetFractal(void) ;
static uint32_t	HSV2RGB(float hue, float sat, float val) ;
static uint32_t	PackRGB(int red, int grn, int blu) ;
static void		PutChar(int x, int y, char c, sFONT *font) ;
static void		PutString(int x, int y, char *str, sFONT *font) ;
static void		BackgroundColor(uint32_t color) ;
static void		ForegroundColor(uint32_t color) ;
static void		FractalTitle(char *title) ;
static int		SanityChecksOK(void) ;

#define ITEMS(a) (sizeof(a)/sizeof(a[0]))
#define	OPAQUE	0xFF000000

static uint32_t foregroundColor = COLOR_BLACK ;
static uint32_t	backgroundColor = COLOR_WHITE ;

int main()
	{
	static void (*fractals[])(void) =
		{
		BarnsleyFernFractal,
		MandelbrotSetFractal,
		JuliaSetFractal
		} ;

	InitializeHardware(HEADER, "Lab 4b: Pixels, Fonts & Fractals") ;
	if (!SanityChecksOK()) exit(255) ;
	for (int fractal = 0;; fractal = (fractal + 1) % ITEMS(fractals))
		{
		(*fractals[fractal])() ;
		WaitForPushButton() ;
		}
	}

static void BarnsleyFernFractal(void)
	{
	const int	LIMIT	= 50000 ;
	const float	X_ZOOM	=   5.5 ;
	const float	Y_ZOOM	=  10.5 ;
	float x = 0 ;
	float y = 0 ;

	SetColor(COLOR_RED) ;
	FillRect(X_MIN, Y_MIN, WIDTH, HEIGHT) ;

	for (int i = 0; i < LIMIT; i++)
		{
		int r = rand() % 100 ;
		float newX, newY ;
		int pxlX, pxlY ;

		if (r <= 1)
			{
			newX = 0.00 ;
			newY = 0.16 * y ;
            }
		else if (r <= 8)
			{
			newX = 0.20 * x - 0.26 * y ;
			newY = 0.23 * x + 0.22 * y + 1.6 ;
            }
		else if (r <= 15)
			{
			newX = -0.15 * x + 0.28 * y ;
			newY =  0.26 * x + 0.24 * y + 0.44 ;
            }
		else
			{
			newX =  0.85 * x + 0.04 * y ;
			newY = -0.04 * x + 0.85 * y + 1.6 ;
            }

		x = newX ;
		y = newY ;

		pxlX = X_MIN + WIDTH/2 + (int) (x * (WIDTH /X_ZOOM)) ;
		pxlY = Y_MIN + HEIGHT  - (int) (y * (HEIGHT/Y_ZOOM)) ;

		*PixelAddress(pxlX, pxlY) = OPAQUE | COLOR_GREEN ;
		}

	FractalTitle("Barnsley Fern") ;
	}

static void MandelbrotSetFractal(void)
	{
	const int	limit	= 128 ;
	const float	X_ZOOM	= 1.30 ;
	const float	Y_ZOOM	= 0.75 ;
	const float	X_OFF	= -0.5 ;
	const float	Y_OFF	= 0.0 ;

	for (int y = 0; y < HEIGHT; y++)
		{
		float py = Y_OFF + (y - HEIGHT/2) * (2.0 / (Y_ZOOM * HEIGHT)) ;
		for (int x = 0; x < WIDTH; x++)
			{
			float px = X_OFF + (x - WIDTH/2) * (3.0 / (X_ZOOM * WIDTH)) ;
			float hue, zx, zy ;
			int iter, rgb ;

			zx = px ; zy = py ;
			for (iter = 0; iter < limit; iter++)
				{
				float zxSquared = zx*zx ;
				float zySquared = zy*zy ;

				if (zxSquared + zySquared > 4.0) break ;

				zy = py + 2.0*zx*zy ;
				zx = px + zxSquared - zySquared ;
				}

			hue = (359.0 * iter) / limit ;
			rgb = HSV2RGB(hue, 1.0, (iter < limit) ? 1.0 : 0.0) ;
			*PixelAddress(x + X_MIN, y + Y_MIN) = rgb ;
			}
		}

	FractalTitle("Mandelbrot Set") ;
	}

static void JuliaSetFractal(void)
	{
	const int	limit	= 128 ;
	const float	X_ZOOM	= 1.20 ;
	const float	Y_ZOOM	= 0.65 ;
	const float	X_OFF	= 0.0 ;
	const float	Y_OFF	= 0.0 ;

	for (int y = 0; y < HEIGHT; y++)
		{
		const float	pY = -0.7 ;
		for (int x = 0; x < WIDTH; x++)
			{
			const float	pX = 0.27015 ;
			float zy = Y_OFF + (y - HEIGHT/2) * (2.0 / (Y_ZOOM * HEIGHT)) ;
			float zx = X_OFF + (x - WIDTH/ 2) * (3.0 / (X_ZOOM * WIDTH )) ;
			int iter, rgb ;
			float hue ;

			for (iter = 0; iter < limit; iter++)
				{
				float zxSquared = zx*zx ;
				float zySquared = zy*zy ;

				if (zxSquared + zySquared > 4) break ;

				zx  = pX + 2.0*zx*zy ;
				zy  = pY + zySquared - zxSquared ;
				}

			hue = (359.0 * iter) / limit ;
			rgb = HSV2RGB(hue, 1.0, (iter < limit) ? 1.0 : 0.0) ;
			*PixelAddress(x + X_MIN, y + Y_MIN) = rgb ;
			}
		}

	FractalTitle("Julia Set") ;
    }

static uint32_t HSV2RGB(float hue, float sat, float val)
	{
	static float p, q, t ;
	float *pRed[] = {&val, &q, &p, &p, &t, &val} ;
	float *pGrn[] = {&t, &val, &val, &q, &p, &p} ;
	float *pBlu[] = {&p, &p, &t, &val, &val, &q} ;
	int i, r, g, b ;
	float f, h ;

	if (sat == 0.0)
		{
		int n = (int) (255.0 * val + 0.5) ;
		return PackRGB(n, n, n) ;
		}

	h = hue / 60.0 ;	// 0.0 <= h < 6.0
	i = (unsigned) h ;	//   0 <= i < 6
	f = h - (float) i ;	//   0 <= f < 6.0

	p = val * (1.0 - sat) ;
	q = val * (1.0 - f*sat) ;
	t = val * (1.0 - (1.0 - f)*sat) ;

	r = (int) (*pRed[i] * 255.0 + 0.5) ;
	g = (int) (*pGrn[i] * 255.0 + 0.5) ;
	b = (int) (*pBlu[i] * 255.0 + 0.5) ;

	return PackRGB(r, g, b) ;
	}

static uint32_t PackRGB(int red, int grn, int blu)
	{
#	define	BYTE(pixel) 	((uint8_t *) &pixel)
	static uint32_t pixel = 0xFF000000 ;

	BYTE(pixel)[0] = blu ;
	BYTE(pixel)[1] = grn ;
	BYTE(pixel)[2] = red ;

	return pixel ;
	}

static void PutChar(int x, int y, char ch, sFONT *font)
	{
	uint8_t *pline = BitmapAddress(ch, (uint8_t *) font->table, font->Height, font->Width) ;
	for (int row = 0; row < font->Height; row++)
		{
		uint32_t bits = GetBitmapRow(pline) ;
		for (int col = 0; col < font->Width; col++)
			{
			uint32_t pixel = ((int32_t) bits < 0) ? (OPAQUE | foregroundColor) : (OPAQUE | backgroundColor) ;
			*PixelAddress(x + col, y + row) = pixel ;
			bits <<= 1 ;
			}
		pline += (font->Width + 7) / 8 ;
		}
	}

static void PutString(int x, int y, char *str, sFONT *font)
	{
	while (*str != '\0')
		{
		PutChar(x, y, *str++, font) ;
		x += font->Width ;
		}
	}

static void BackgroundColor(uint32_t color)
	{
	backgroundColor = color ;
	}

static void ForegroundColor(uint32_t color)
	{
	foregroundColor = color ;
	}

static void FractalTitle(char *title)
	{
	sFONT *font = &Font16 ;
	int width = strlen(title) * font->Width ;
	int xpos = (XPIXELS - width) / 2 ;
	int ypos = YPIXELS - 16 - font->Height ;

	ForegroundColor(COLOR_WHITE) ;
	BackgroundColor(COLOR_RED) ;
	PutString(xpos, ypos, title, font) ;
	}

static int SanityChecksOK(void)
	{
	extern void BSP_LCD_SetFont(sFONT *) ;
	sFONT *font = &Font16 ;
	int ttl, row, col, bugs ;
	unsigned bits,need ;
	char text[100] ;
	void *adrs ;

    BSP_LCD_SetFont(font);

	bugs = 0 ;
	col = 15 ;

	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_WHITE) ;

	ttl = 60 ;
	row = ttl + font->Height + 4 ;
	adrs = BitmapAddress('H', (uint8_t *) Font8.table, Font8.Height, Font8.Width) ;
	bits = *(uint32_t *) adrs ;
	need = 0x487848E8 ;
	if (bits != need)
		{
		DisplayStringAt(col, row, "Character: H") ;
		row += font->Height ;
		DisplayStringAt(col, row, "   Height: 8") ;
		row += font->Height ;
		DisplayStringAt(col, row, "    Width: 5") ;
		row += font->Height ;
		sprintf(text, "  Address: %08X", (unsigned) adrs) ;
		DisplayStringAt(col, row, text) ;
		row += font->Height ;
		sprintf(text, " Contents: %08X", bits) ;
		DisplayStringAt(col, row, text) ;
		row += font->Height ;
		sprintf(text, "Should be: %08X", need) ;
		DisplayStringAt(col, row, text) ;
		row += 2*font->Height ;
		bugs++ ;
		}

	adrs = BitmapAddress('/', (uint8_t *) Font24.table, Font24.Height, Font24.Width) ;
	bits = *(uint32_t *) adrs ;
	need = 0x00001800 ;
	if (bits != need)
		{
		DisplayStringAt(col, row, "Character: /") ;
		row += font->Height ;
		DisplayStringAt(col, row, "   Height: 24") ;
		row += font->Height ;
		DisplayStringAt(col, row, "    Width: 17") ;
		row += font->Height ;
		sprintf(text, "  Address: %08X", (unsigned) adrs) ;
		DisplayStringAt(col, row, text) ;
		row += font->Height ;
		sprintf(text, " Contents: %08X", bits) ;
		DisplayStringAt(col, row, text) ;
		row += font->Height ;
		sprintf(text, "Should be: %08X", need) ;
		DisplayStringAt(col, row, text) ;
		row += 2*font->Height ;
		bugs++ ;
		}

	if (bugs > 0)
		{
		SetForeground(COLOR_WHITE) ;
		SetBackground(COLOR_RED) ;
		DisplayStringAt(col, ttl, "BitmapAddress Bugs:") ;
		ttl = row + font->Height ;
		row = ttl + font->Height + 4 ;
		}

	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_WHITE) ;

	adrs = PixelAddress(0, 0) ;
	need = 0xD0000000 ;
	if (bugs < 2 && adrs != (void *) need)
		{
		DisplayStringAt(col, row, "  x (col): 0") ;
		row += font->Height ;
		DisplayStringAt(col, row, "  y (row): 0") ;
		row += font->Height ;
		sprintf(text, " Returned: %08X", (unsigned) adrs) ;
		DisplayStringAt(col, row, text) ;
		row += font->Height ;
		sprintf(text, "Should be: %08X", need) ;
		DisplayStringAt(col, row, text) ;
		row += 2*font->Height ;
		bugs++ ;
		}

	adrs = PixelAddress(239, 319) ;
	need = 0xD004AFFC ;
	if (bugs < 2 && adrs != (void *) need)
		{
		DisplayStringAt(col, row, "  x (col): 239") ;
		row += font->Height ;
		DisplayStringAt(col, row, "  y (row): 319") ;
		row += font->Height ;
		sprintf(text, " Returned: %08X", (unsigned) adrs) ;
		DisplayStringAt(col, row, text) ;
		row += font->Height ;
		sprintf(text, "Should be: %08X", need) ;
		DisplayStringAt(col, row, text) ;
		row += 2*font->Height ;
		bugs++ ;
		}

	if (bugs <= 2)
		{
		SetForeground(COLOR_WHITE) ;
		SetBackground(COLOR_RED) ;
		DisplayStringAt(col, ttl, "PixelAddress Bugs:") ;
		}

	return bugs == 0 ;
	}

