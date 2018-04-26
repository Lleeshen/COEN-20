/*
	This code was written to support the book, "ARM Assembly for Embedded Applications",
	by Daniel W. Lewis. Permission is granted to share this software without permnission
	provided that this notice is not removed. This software is intended to be used with
	a special run-time library written by the author for the EmBitz IDE, using the
	"Sample Program for the 32F429IDISCOVERY Board" as an example, and available for
	download from http://www.engr.scu.edu/~dlewis/book3.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>
#include "library.h"
#include "graphics.h"

extern float	Bin2Dec(const int bin[8]) ;
extern void		Dec2Bin(const float x, int bin[8]) ;
extern void		TwosComplement(const int input[8], int output[8]) ;

#define	ROW_HEIGHT	14
#define ENTRIES(a)	(sizeof(a)/sizeof(a[0]))

#define	FIRST_ROW	69
#define	LABEL_COL	20
#define	VALUE_COL	138

typedef struct
	{
	float	dec ;
	int		bin ;
	int		neg ;
	} TESTCASE ;

int	PutBinAt(int row, int col, int bin[8], int check) ;
int PutDecAt(int row, int col, float dec, float check) ;
int	PutStringAt(int row, int col, char *fmt, ...) ;

int main(void)
	{
	TESTCASE testcase[] =
		{
		{-1.000,	-128,	-128},
		{+0.000,	   0,	   0},
		{+0.010,	   1,	  -1},
		{+0.030,	   4,	  -4},
		{+0.070,	   9,	  -9},
		{+0.100,	  13,	 -13},
		{+0.300,	  38,	 -38},
		{+0.700,	  90,	 -90}
		} ;
	TESTCASE *t ;
	int k, row ;

	InitializeHardware(HEADER, "LAB 1: Binary Number Systems") ;

	row = FIRST_ROW ;
	for (k= 0;; k = (k + 1) % ENTRIES(testcase))
		{
		float dec, neg, abserr, percent ;
		int bin[8] ;

		t = &testcase[k] ;
		SetColor(COLOR_BLACK) ;

		row = PutStringAt(row, LABEL_COL, "   Dec Test %d/%d: %+f", k+1, ENTRIES(testcase), t->dec) ;
		DrawHLine(LABEL_COL, row, XPIXELS - 2*LABEL_COL) ;
		row += ROW_HEIGHT ;

		PutStringAt(row, LABEL_COL, "       DecToBin:") ;
		Dec2Bin(t->dec, bin) ;
		row = PutBinAt(row, VALUE_COL, bin, t->bin) ;

		PutStringAt(row, LABEL_COL, "       BinToDec:") ;
		dec = Bin2Dec(bin) ;
		row = PutDecAt(row, VALUE_COL, dec, t->bin/128.0) ;

		row += ROW_HEIGHT ;

		abserr = fabs(t->dec - dec) ;
		row = PutStringAt(row, LABEL_COL, " Absolute Error:  %f", abserr) ;

		if (fabs(t->dec) == 0) percent = 0 ;
		else percent = 100 * abserr / fabs(t->dec) ;
		row = PutStringAt(row, LABEL_COL, " Relative Error: %5.2f%c", percent, '%') ;

		row += ROW_HEIGHT ;

		TwosComplement(bin, bin) ;
		PutStringAt(row, LABEL_COL, " 2's complement:") ;
		row = PutBinAt(row, VALUE_COL, bin, t->neg) ;

		PutStringAt(row, LABEL_COL, "       BinToDec:") ;
		neg = Bin2Dec(bin) ;
		row = PutDecAt(row, VALUE_COL, neg, t->neg/128.0) ;

		Dec2Bin(neg, bin) ;
		PutStringAt(row, LABEL_COL, "       DecToBin:") ;
		row = PutBinAt(row, VALUE_COL, bin, t->neg) ;

		row += ROW_HEIGHT ;

		row = PutStringAt(row, LABEL_COL, "    Blue Button:  Next Test") ;
		WaitForPushButton() ;
		ClearDisplay() ;
		row = FIRST_ROW ;
		}

	return 0 ;
	}

int PutStringAt(int row, int col, char *fmt, ...)
	{
	va_list args ;
	char text[100] ;

	va_start(args, fmt) ;
	vsprintf(text, fmt, args) ;
	va_end(args) ;

	DisplayStringAt(col, row, text) ;

	return row + ROW_HEIGHT ;
	}

int PutDecAt(int row, int col, float dec, float check)
	{
	if (dec != check)
		{
		SetForeground(COLOR_WHITE) ;
		SetBackground(COLOR_RED) ;
		}
	row = PutStringAt(row, col, "%+f", dec) ;
	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_WHITE) ;
	return row ;
	}

int PutBinAt(int row, int col, int bin[8], int check)
	{
	char text[20], *p ;
	int k, num ;

	sprintf(text, " %d.", bin[7]) ;
	p = text + 3 ;

	num = -bin[7] ;
	for (k = 6; k >= 0; k--)
		{
		*p++ = '0' + bin[k] ;
		num = 2*num + bin[k] ;
		}
	*p = '\0' ;

	if (check == 128) check = -128 ;
	if (num != check)
		{
		SetForeground(COLOR_WHITE) ;
		SetBackground(COLOR_RED) ;
		}
	row = PutStringAt(row, col, text) ;
	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_WHITE) ;
	return row ;
	}
