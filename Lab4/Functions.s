            .syntax unified
            .cpu cortex-m4
            .text


            //uint32_t *PixelAddress(int x, int y)
            //Returns the address of the 32-bit word
            //for the pixel at position x,y
            .global PixelAddress
            .thumb_func
            .align

PixelAddress:
            //R0 = x, R1 = y
            LDR R2,=240
            // R2 = 240
            LDR R3,=0xD0000000
            // R3 is the first address
            MLA R0, R2, R1, R0
            //R0 = x + 240y
            ADD R0,R3,R0,LSL 2
            BX LR

            //uint8_t *BitmapAddress(char ascii, uint8_t *fontTable,
            //int charHeight, int charWidth)

            //Returns the address of the bitmap for a specified character
            //within a given font table
            .global BitmapAddress
            .thumb_func
            .align

BitmapAddress:
            //R0 = ascii, R1 = fontTable, R2 = charHeight, R3 = charWidth
            ADD R3,R3,7
            LSR R3,R3,3
            //R3 is now the number of bytes per row
            SUB R0,R0,' '
            //R0 is number of ascii characters after space
            MUL R2,R3,R2
            //R2 = number of bytes = number of bytes per row * height
            MLA R0,R2,R0,R1
            //R0 = fontTable + ascii chars * number of bytes
            BX LR

            //uint32_t GetBitmapRow(uint8_t *rowAdrs)

            //Returns a left-justified row of bits corresponding to the pixels
            //in one row of a character.
            .global GetBitmapRow
            .thumb_func
            .align

GetBitmapRow:
            LDR R1,[R0]
            REV R0,R1
            BX LR
            .end
