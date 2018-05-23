/*
	This code was written to support the book, "ARM Assembly for Embedded Applications",
	by Daniel W. Lewis. Permission is granted to freely share this software provided
	that this notice is not removed. This software is intended to be used with a
	special run-time library written by the author for the EmBitz IDE, using the
	"Sample Program for the 32F429IDISCOVERY Board" as an example, and available for
	download from http://www.engr.scu.edu/~dlewis/book3.
*/

// The following code was adapted from algorithms found
// in Chapter 10 of "Hacker's Delight" by Henry S. Warren,
// Upper Saddle River, NJ: Addison-Wesley, 2013.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

typedef struct
	{
	int			multiplier ;
	int			shiftcount ;
	} SMAGIC ;

typedef struct
	{
	unsigned	multiplier ;
	int			shiftcount ;
	int			insert_add ;
	} UMAGIC ;

int PowerOf2(int divisor)
	{
	return (abs(divisor) & (abs(divisor) - 1)) == 0 ;
	}

char *Cycles(char *mnemonic)
	{
	if (strstr("PUSH,POP,LDMIA,LDMDB,STMIA,STMDB", mnemonic) != NULL) return "1 + #regs" ;
	if (strstr("SDIV,UDIV", mnemonic) != NULL) return "2-12" ;
	if (strstr("B,BL,BX", mnemonic) != NULL) return "2-4" ;
	if (mnemonic[0] == 'B' && strlen(mnemonic) == 3) return "Success: 2-4, Fail: 1" ;
	if (strstr("ADR,MLA,MLS,LDR,LDRB,LDRH,LDRSB,LDRSH,STR,STRB,STRH", mnemonic) != NULL) return "2" ;
	if (strstr("SMMUL,SMMULR,SMMLA,SMMLAR,SMMLS,SMMLSR", mnemonic) != NULL) return "2" ;
	if (strstr("LDRD,STRD", mnemonic) != NULL) return "3" ;
	return "1" ;
	}

void Emit(char *mnemonic, char *opformat, ...)
	{
	char operands[100] ;
	va_list args ;

	va_start(args, opformat) ;
	vsprintf(operands, opformat, args) ;
	va_end(args) ;

	printf("\t%-8s%-20s// %s clock cycle(s)\n", mnemonic, operands, Cycles(mnemonic)) ;
	}

void EmitAdjustedASR(int divisor)
	{
	unsigned temp = abs(divisor) ;
	int shiftcount ;

	printf("\t// Divisor IS power of 2 - adjust dividend and use ASR\n\n") ;
	for (shiftcount = -1; temp != 0; temp >>= 1) shiftcount++ ;

	Emit("LDR", "R0,dividend") ;
	if (shiftcount > 1)
		{
		Emit("ASRS.N", "R1,R0,%d", shiftcount - 1) ;
		Emit("ADD", "R0,R0,R1,LSR %d", 32 - shiftcount) ;
		}
	else Emit("ADD", "R0,R0,R0,LSR 31") ;
	Emit("ASRS.N", "R0,R0,%d", shiftcount) ;
	if (divisor < 0) Emit("RSBS.N", "R0,R0,0") ;
	Emit("STR", "R0,quotient") ;
	}

SMAGIC SignedMagic(int divisor)
	{
	unsigned ad, anc, delta, q1, r1, q2, r2, t ;
	SMAGIC magic ;
	int p ;

	ad  = abs(divisor) ;
	t   = 0x80000000 + ((unsigned) divisor >> 31) ;
	anc = t - 1 - (t % ad) ;

	q1 = 0x80000000 / anc ;
	r1 = 0x80000000 - q1 * anc ;

	q2 = 0x80000000 / ad ;
	r2 = 0x80000000 - q2 * ad ;

	p = 31 ;
	do
		{
		p++ ;

		q1 *= 2 ; r1 *= 2 ;
		if (r1 >= anc) { q1++ ; r1 -= anc ;	}

		q2 *= 2 ; r2 *= 2 ;
		if (r2 >= ad)  { q2++ ; r2 -= ad ;  }

		delta = ad - r2 ;

		} while (q1 < delta || (q1 == delta && r1 == 0)) ;

	magic.multiplier = q2 + 1 ;
	if (divisor < 0) magic.multiplier = -magic.multiplier ;
	magic.shiftcount = p - 32 ;
	return magic ;
	}

void EmitSigned(int divisor)
	{
	SMAGIC magic = SignedMagic(divisor) ;

	printf("\t// Signed division by other than power of 2\n\n") ;
	Emit("LDR", "R0,dividend") ;
	Emit("LDR", "R1,=0x%08X", magic.multiplier) ;

//	Emit("SMULL", "R2,R1,R1,R0") ;
//	if (divisor > 0 && magic.multiplier < 0) Emit("ADD", "R1,R1,R0") ;
//	if (divisor < 0 && magic.multiplier > 0) Emit("SUB", "R1,R1,R0") ;

	if (divisor > 0 && magic.multiplier < 0) Emit("SMMLA", "R1,R1,R0,R0") ;
	else if (divisor < 0 && magic.multiplier > 0) Emit("SMMLS", "R1,R1,R0,R0") ;
	else Emit("SMMUL", "R1,R1,R0") ;

	if (magic.shiftcount != 0) 		         Emit("ASRS.N", "R1,R1,%d", magic.shiftcount) ;
	if (divisor > 0) Emit("ADD", "R0,R1,R0,LSR 31") ;
	else			 Emit("ADD", "R0,R1,R1,LSR 31") ;
	Emit("STR", "R0,quotient") ;
	}

UMAGIC UnsignedMagic(unsigned divisor)
	{
	unsigned nc, delta, q1, r1, q2, r2 ;
	UMAGIC magic ;
	int p ;

	nc = -1 - (-divisor) % divisor ;

	q1 = 0x80000000 / nc ;
	r1 = 0x80000000 - q1 * nc ;

	q2 = 0x7FFFFFFF / divisor ;
	r2 = 0x7FFFFFFF - q2 * divisor ;

	magic.insert_add = 0 ;
	p = 31 ;
	do
		{
		p++ ;

		if (r1 >= nc - r1) { q1 = 2*q1 + 1 ; r1 = 2*r1 - nc; }
		else { q1 = 2*q1 ; r1 = 2*r1 ; }

		if (r2 + 1 >= divisor - r2)
			{
			if (q2 >= 0x7FFFFFFF) magic.insert_add = 1 ;
			q2 = 2*q2 + 1 ;
			r2 = 2*r2 + 1 - divisor ;
			}
		else
			{
			if (q2 >= 0x80000000) magic.insert_add = 1 ;
			q2 = 2*q2 ;
			r2 = 2*r2 + 1 ;
			}

		delta = divisor - 1 - r2 ;

		} while (p < 64 &&
			(q1 < delta || (q1 == delta && r1 == 0))) ;

	magic.multiplier = q2 + 1 ;
	magic.shiftcount = p - 32 ;
	return magic ;
	}

void EmitUnsigned(int divisor)
	{
	UMAGIC magic = UnsignedMagic(divisor) ;

	printf("\t// Unsigned division by other than power of 2\n\n") ;
	Emit("LDR", "R0,dividend") ;
	Emit("LDR", "R1,=0x%08X", magic.multiplier) ;
	Emit("UMULL", "R2,R1,R1,R0") ;
	if (magic.shiftcount != 0)
			{
			if (magic.insert_add != 0)
				{
				Emit("ADDS.N", "R1,R1,R0") ;
				Emit("RRX", "R1,R1") ;
				magic.shiftcount--;
				}
			Emit("LSRS.N", "R0,R1,%d", magic.shiftcount) ;
			}
	else if (magic.insert_add != 0)
		{
		Emit("ADDS.N", "R0,R0,R1") ;
		}
	Emit("STR", "R0,quotient") ;
	}

int main()
	{
	int signedDivide ;
	char input[10] ;
	int divisor ;

	for (;;)
		{
		printf("Enter constant divisor (default is unsigned, use +/- for signed, 0 to exit): ") ;
		scanf("%s", input) ;
		signedDivide = strpbrk(input, "+-") != NULL ;
		sscanf(input, "%d", &divisor) ;
		printf("\n") ;
		if (abs(divisor) <= 1) break ;

		if (PowerOf2(divisor)) EmitAdjustedASR(divisor) ;
		else if (signedDivide) EmitSigned(divisor) ;
		else EmitUnsigned(divisor) ;

		printf("\n") ;
		}

	return 0 ;
	}
