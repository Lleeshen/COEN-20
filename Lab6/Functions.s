            .syntax unified
            .cpu cortex-m4
            .text


            //void PutNibble(void *nibbles, uint32_t index, uint32_t nibble) ;
            //Insert a nibble, a 4-bit value, into the array of nibbles with specified index for nibbles
            .global PutNibble
            .thumb_func
            .align

PutNibble:
            LSRS R1,R1,1         //R1 = index /2, remainder in carry flag
            LDRB R3, [R0, R1]    //R3 = byte with desired nibble

            ITTE CS
            ANDCS R3,R3,0x0F     //Clear most significant nibble if flag=1
            LSLCS R2,R2,4        //Move nibble data to most significant nibble
            ANDCC R3,R3,0xF0     //Clear least significant nibble if flag = 0

            ORR R2, R2, R3
            STRB R2,[R0,R1]

            BX LR

            //uint32_t GetNibble(void *nibbles, uint32_t index) ;
            //Return the nipple in the index of nibbles array
            .global GetNibble
            .thumb_func
            .align

GetNibble:
            LSRS R1,R1,1    //index = index /2, remainder in carry flag
            LDRB R3,[R0,R1]

            ITE CS
            LSRCS R3, 4
            ANDCC R3,R3,0x0F

            MOV R0,R3
            BX LR
            .end
