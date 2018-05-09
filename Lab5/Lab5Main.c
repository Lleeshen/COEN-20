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
#include <stdlib.h>
#include "library.h"
#include "graphics.h"

typedef int BOOL ;
#define	TRUE	1
#define	FALSE	0

extern void DeleteItem(int data[], int items, int index) ;
extern void InsertItem(int data[], int items, int index, int value) ;

#define ENTRIES(a) (sizeof(a)/sizeof(a[0]))

void InitArray(int data[], int items) ;
void CopyArray(int dst[], int src[], int items) ;
BOOL DelOK(int orig[], int del[], int row, int index) ;
BOOL InsOK(int orig[], int del[], int row, int index) ;
void DisplayResults(int orig[], int del[], int ins[], int index) ;
int CellColor(int row, int index, BOOL del, BOOL ins) ;

#define	VALUE_INS	0
#define	ITEMS		10

int main(void)
	{
	unsigned int strt, stop, overhead, cycles ;
	int orig[ITEMS], del[ITEMS], ins[ITEMS+1], index ;

	InitializeHardware(HEADER, "Lab 5: Inserting & Deleting") ;

	strt = GetClockCycleCount() ;
	stop = GetClockCycleCount() ;
	overhead = stop - strt ;

	index = 0 ;
	while (1)
		{
		char text[100] ;

		InitArray(orig, ITEMS) ;

		CopyArray(del, orig, ITEMS) ;
		strt = GetClockCycleCount() ;
		DeleteItem(del, ITEMS, index) ;
		stop = GetClockCycleCount() ;
		cycles = stop - strt - overhead ;
		sprintf(text, "Del at index %d: %d Clock Cycles", index, cycles) ;
		DisplayStringAt(5, 55, text) ;

		CopyArray(ins, orig, ITEMS) ;
		ins[ITEMS] = -1 ; // Must not overwrite this!
		strt = GetClockCycleCount() ;
		InsertItem(ins, ITEMS, index, VALUE_INS) ;
		stop = GetClockCycleCount() ;
		cycles = stop - strt - overhead ;
		sprintf(text, "Ins at index %d: %d Clock Cycles", index, cycles) ;
		DisplayStringAt(5, 70, text) ;

		DisplayResults(orig, del, ins, index) ;

		DisplayStringAt(40, 285, "Blue Button: Next Index") ;
		WaitForPushButton() ;

		index = (index + 1) % ITEMS ;
		}

	while (1) ;
	return 0 ;
	}

void InitArray(int data[], int items)
	{
	int k ;

	for (k = 0; k < ITEMS; k++)
		{
		int value ;

		do value = (int) GetRandomNumber() % 1000 ;
		while (value < 100) ;
		data[k] = value ;
		}
	}

void CopyArray(int dst[], int src[], int items)
	{
	int k ;

	for (k = 0; k < items; k++)
		{
		dst[k] = src[k] ;
		}
	}

int CellColor(int row, int index, BOOL del, BOOL ins)
	{
	if (row < index) return COLOR_LIGHTGREEN ;

	if (del && row == ITEMS - 1) return COLOR_WHITE ;
	if (del && row == index) return COLOR_LIGHTCYAN ;

	if (ins && row == index) return COLOR_WHITE ;
	if (ins && row == index + 1) return COLOR_YELLOW ;

	if (row > index) return COLOR_LIGHTCYAN ;

	return COLOR_YELLOW ;
	}

void DisplayResults(int orig[], int del[], int ins[], int index)
	{
	int row, x, y, ok ;
	char text[10], *marker ;

	DisplayStringAt(17, 98, "Index  Del <-- Orig --> Ins") ;
	DrawHLine(17, 113, XPIXELS - 45) ;

	for (row = 0; row < ITEMS; row++)
		{
		x = 5 ;
		y = 15*row + 117 ;

		sprintf(text, "%3d", row) ;
		SetForeground(COLOR_BLACK) ;
		SetBackground(COLOR_WHITE) ;
		DisplayStringAt(x + 8, y, text) ;

		// After the delete
		x += XPIXELS/4 ;
		ok = DelOK(orig, del, row, index) ;
		sprintf(text, "%3d", del[row]) ;
		SetForeground(ok ? COLOR_BLACK : COLOR_WHITE) ;
		SetBackground(ok ? CellColor(row, index, TRUE, FALSE) : COLOR_RED) ;
		DisplayStringAt(x, y,  text) ;

		x += XPIXELS/4 ;
		SetForeground(COLOR_BLACK) ;
		SetBackground(COLOR_WHITE) ;
		marker = (row > index) ? "up" : "  " ;
		DisplayStringAt(x - 20, y, marker) ;

		// Original array contents
		sprintf(text, "%3d", orig[row]) ;
		SetForeground(COLOR_BLACK) ;
		SetBackground(CellColor(row, index, FALSE, FALSE)) ;
		DisplayStringAt(x, y, text) ;

		SetForeground(COLOR_BLACK) ;
		SetBackground(COLOR_WHITE) ;
		marker = (row >= index && row != ITEMS - 1) ? "dn" : "  " ;
		DisplayStringAt(x + 27, y, marker) ;

		// After the insert
		x += XPIXELS/4 ;
		ok = InsOK(orig, ins, row, index) ;
		sprintf(text, "%3d", ins[row]) ;
		SetForeground(ok ? COLOR_BLACK : COLOR_WHITE) ;
		SetBackground(ok ? CellColor(row, index, FALSE, TRUE) : COLOR_RED) ;
		DisplayStringAt(x, y, text) ;
		}

	if (ins[ITEMS] != -1)
		{
		SetForeground(COLOR_WHITE) ;
		SetBackground(COLOR_RED) ;
		DisplayStringAt(15, 270, "Insert wrote beyond the array!") ;
		}

	SetForeground(COLOR_BLACK) ;
	SetBackground(COLOR_WHITE) ;
	}

BOOL DelOK(int orig[], int del[], int row, int index)
	{
	if (row < index && del[row]==orig[row]) return TRUE ;
	if (row == ITEMS-1 && del[row]==orig[row])  return TRUE ;
	return del[row] == orig[row+1] ;
	}

BOOL InsOK(int orig[], int ins[], int row, int index)
	{
	if (row < index && ins[row]==orig[row])  return TRUE ;
	if (row == index && ins[row]==VALUE_INS) return TRUE ;
	return row < ITEMS && ins[row]==orig[row-1] ;
	}




