            .syntax unified
            .cpu cortex-m4
            .text


            //void CallReturnOverhead
            //Return nothing, used to test runtime performance
            .global CallReturnOverhead
            .thumb_func
            .align

CallReturnOverhead:
            BX LR

            //int32_t SDIVby13(int21_t dividend)
            //Load 13 into register and use signed divide to divide by 13
            .global SDIVby13
            .thumb_func
            .align

SDIVby13:
            MOVS.N R1,13
            SDIV R0,R0,R1
            BX LR

            //void SDIVby13(int21_t dividend)
            //Load 13 into register and use signed divide to divide by 13
            .global UDIVby13
            .thumb_func
            .align

UDIVby13:
            MOVS.N R1,13
            UDIV R0,R0,R1
            BX LR

            //int32_t MySDIVby13(int32_t dividend) ;
            //Optimize signed divide by 13 using multiply
            .global MySDIVby13
            .thumb_func
            .align

MySDIVby13:

            LDR R1,=0x4EC4EC4F
            SMMUL R1,R1,R0
            ASRS.N R1,R1,2
            ADD R0,R1,R0,LSR 31
            BX LR

            //int32_t MyUDIVby13(int32_t dividend) ;
            //Optimize unsigned divide by 13 using multiply
            .global MyUDIVby13
            .thumb_func
            .align

MyUDIVby13:

            LDR R1,=0x4EC4EC4F
            UMULL R2,R1,R1,R0
            LSRS.N R0,R1,2
            BX LR

            .end
