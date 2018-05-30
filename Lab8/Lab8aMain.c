/*
	This code was written to support the book, "ARM Assembly for Embedded Applications",
	by Daniel W. Lewis. Permission is granted to freely share this software provided
	that this notice is not removed. This software is intended to be used with a
	special run-time library written by the author for the EmBitz IDE, using the
	"Sample Program for the 32F429IDISCOVERY Board" as an example, and available for
	download from http://www.engr.scu.edu/~dlewis/book3.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "library.h"
#include "graphics.h"

#define	COLOR_FLOAT		COLOR_MAGENTA
#define	COLOR_FIXED		COLOR_ORANGE

typedef int32_t Q16 ;

#define	ENTRIES(a)		(sizeof(a)/sizeof(a[0]))

#define Q16ONE			0x00010000
#define	FloatToQ16(x)	((Q16) (x * ((float) Q16ONE)))
#define Q16ToFloat(x)	(((float) x) / ((float) Q16ONE))

extern float			FloatPoly(float x, float coef[], int32_t numb) ;
extern Q16				FixedPoly(Q16   x, Q16   coef[], int32_t numb) ;

static float			FloatSine(float radians, unsigned overhead) ;
static float			FixedSine(float radians, unsigned overhead) ;
static void				PutStringAt(int row, int col, char *fmt, ...) ;
static void				PutErrMsgAt(int row, int col, char *fmt, ...) ;

#define	ROW1			50
#define	ROWN			(YPIXELS-50)
#define	ROWS			(ROWN - ROW1 + 1)
#define COLS			XPIXELS
#define	HEIGHT			(ROWS/4)
#define DEGREES			540
#define	TXT_YPXLS		15

#define	FLT_GFXROW		(ROW1 + 1*ROWS/4)
#define	FXD_GFXROW		(ROW1 + 3*ROWS/4 + TXT_YPXLS + 2)

#define	FLT_TXTROW		(FLT_GFXROW + ROWS/4 + 2)
#define	FXD_TXTROW		(FXD_GFXROW + ROWS/4 + 2)
#define	MSG_TXTCOL		7

int main(void)
	{
	int16_t oldFltRow[COLS], oldFxdRow[COLS] ;
	unsigned strt, stop , overhead ;
    int16_t row, col, phase ;

    InitializeHardware(HEADER, "Lab 8a: Fixed vs Floating Point") ;

	strt = GetClockCycleCount() ;
	stop = GetClockCycleCount() ;
	overhead = stop - strt ;

	for (col = 0; col < COLS; col++)
		{
		oldFltRow[col] = FLT_GFXROW ;
		oldFxdRow[col] = FXD_GFXROW ;
		}

	phase = 0 ;
	while (1)
		{
		int16_t *pOldFltRow, *pOldFxdRow ;

		pOldFltRow = &oldFltRow[0] ;
		pOldFxdRow = &oldFxdRow[0] ;

		for (col = 0; col < COLS; col++)
			{
			float radians, floatSine, fixedSine ;
			int16_t angle, degrees ;

			degrees = (DEGREES * col + COLS/2) / COLS ;
			degrees = (degrees + phase) % 360 ;

			if (degrees >= 270)			angle = 360 - degrees ;
			else if (degrees >= 180)	angle = degrees - 180 ;
			else if (degrees >= 90)		angle = 180 - degrees ;
			else						angle = degrees ;

			radians = (3.14158 * angle) / 180 ;

			floatSine = FloatSine(radians, overhead) ;
			fixedSine = FixedSine(radians, overhead) ;

			if (degrees >= 180)
				{
				floatSine = -floatSine ;
				fixedSine = -fixedSine ;
				}

			row = FLT_GFXROW + HEIGHT * floatSine ;
			SetForeground(((row < *pOldFltRow) == (row < FLT_GFXROW)) ? COLOR_FLOAT : COLOR_WHITE) ;
			DrawLine(col, row, col, *pOldFltRow) ;
			*pOldFltRow++ = row ;

			row = FXD_GFXROW + HEIGHT * fixedSine ;
			SetForeground(((row < *pOldFxdRow) == (row < FXD_GFXROW)) ? COLOR_FIXED : COLOR_WHITE) ;
			DrawLine(col, row, col, *pOldFxdRow) ;
			*pOldFxdRow++ = row ;
			}

		if (--phase <= 0) phase = 360 ;
		}

	return 0 ;
	}

static float FloatSine(float radians, unsigned overhead)
    {
	static unsigned total = 0, count = 0 ;
	static int error = 0 ;
	unsigned strt, stop ;
    float coef[] =
		{
		 0.0,					//  0/0!
		+1.0/(1),				// +1/1!
		 0.0,					//  0/2!
		-1.0/(3*2*1),			// -1/3!
		 0.0,					//  0/4!
		+1.0/(5*4*3*2*1),		// +1/5!
		 0.0,					//  0/6!
		-1.0/(7*6*5*4*3*2*1)	// -1/7!
		} ;
	float sine ;

	strt = GetClockCycleCount() ;
    sine = FloatPoly(radians, coef, ENTRIES(coef)) ;
	stop = GetClockCycleCount() ;

	total += stop - strt ;
	count++ ;

	if (sine < 0.0)
		{
		if (!error) PutErrMsgAt(FLT_TXTROW, MSG_TXTCOL, "FloatPoly: sin(%4.2f radians) < 0", radians) ;
		error = 1 ;
		sine = 0.0 ;
		}
	else if (sine > 1.0)
		{
		if (!error) PutErrMsgAt(FLT_TXTROW, MSG_TXTCOL, "FloatPoly: sin(%4.2f radians) > 1", radians) ;
		error = 1 ;
		sine = 1.0 ;
		}
	else if (!error && radians == 0.0)
		{
		PutStringAt(FLT_TXTROW, MSG_TXTCOL, "FloatPoly: %3u clock cycles (avg)", (total + count/2)/count - overhead) ;
		total = count = 0 ;
		}

	return sine ;
    }

static float FixedSine(float radians, unsigned overhead)
    {
	static unsigned total = 0, count = 0 ;
	static int error = 0 ;
	const Q16 one = 0x00010000 ;
	unsigned strt, stop ;
    static Q16 coef[] =
		{
		0x00000000,	//  0/0!
		0x00010000,	// +1/1!
		0x00000000,	//  0/2!
		0xFFFFD555,	// -1/3!
		0x00000000,	//  0/4!
		0x00000222,	// +1/5!
		0x00000000,	//  0/6!
		0xFFFFFFF3	// -1/7!
		} ;
	Q16 sine, Q16Radians ;

	Q16Radians = FloatToQ16(radians) ;

	strt = GetClockCycleCount() ;
    sine = FixedPoly(Q16Radians, coef, ENTRIES(coef)) ;
	stop = GetClockCycleCount() ;

	total += stop - strt ;
	count++ ;

	if (sine < 0)
		{
		if (!error) PutErrMsgAt(FXD_TXTROW, MSG_TXTCOL, "FixedPoly: sin(%4.2f radians) < 0", radians) ;
		error = 1 ;
		sine = 0 ;
		}
	else if (sine > one)
		{
		if (!error) PutErrMsgAt(FXD_TXTROW, MSG_TXTCOL, "FixedPoly: sin(%4.2f radians) > 1", radians) ;
		error = 1 ;
		sine = one ;
		}
	else if (!error && radians == 0)
		{
		PutStringAt(FXD_TXTROW, MSG_TXTCOL, "FixedPoly: %3u clock cycles (avg)", (total + count/2)/count - overhead) ;
		total = count = 0 ;
		}

	return Q16ToFloat(sine) ;
    }

static void PutStringAt(int row, int col, char *fmt, ...)
	{
	va_list args ;
	char text[100] ;

	va_start(args, fmt) ;
	vsprintf(text, fmt, args) ;
	va_end(args) ;

	SetForeground(COLOR_BLACK) ;
	DisplayStringAt(col, row, text) ;
	}

static void PutErrMsgAt(int row, int col, char *fmt, ...)
	{
	va_list args ;
	char text[100] ;

	va_start(args, fmt) ;
	vsprintf(text, fmt, args) ;
	va_end(args) ;

	SetForeground(COLOR_WHITE) ;
	SetBackground(COLOR_RED) ;
	DisplayStringAt(col, row, text) ;
	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_WHITE) ;
	}




