            .syntax unified
            .cpu cortex-m4
            .text


            //void UseLDRB(void *dst, void *src)
            //Copy 512 bytes, 1 byte at a time, from src to dst
            .global UseLDRB
            .thumb_func
            .align

UseLDRB:    .rept 512
            LDRB R3,[R1],1
            STRB R3,[R0],1
            .endr

            BX LR

            //void UseLDRH(void *dst, void *src)
            //Copies 512 bytes, 2 bytes at a time, from src to dst
            .global UseLDRH
            .thumb_func
            .align

UseLDRH:
            .rept 256
            LDRH R3,[R1],2
            STRH R3,[R0],2
            .endr

            BX LR

            //UseLDR(void *dst, void *src)
            //Copies 512 bytes, 4 bytes at a time, from src to dst
            .global UseLDR
            .thumb_func
            .align

UseLDR:
            .rept 128
            LDR R3,[R1],4
            STR R3,[R0],4
            .endr

            BX LR

            //UseLDRD(void *dst,void *src)
            //Copies 512 bytes, 8 bytes at a time, from src to dst
            .global UseLDRD
            .thumb_func
            .align

UseLDRD:
            .rept 64
            LDRD R3,R2,[R1],8
            STRD R3,R2,[R0],8
            .endr
            BX LR

            //UseLDMIA(void *dst,void *src)
            //Copies 512 bytes, 32 bytes at a time, from src to dst
            .global UseLDMIA
            .thumb_func
            .align

UseLDMIA:
            PUSH {R4-R9}
            .rept 16
            LDMIA R1!,{R2-R9}
            STMIA R0!,{R2-R9}
            .endr
            POP {R4-R9}
            BX LR

            .end
`
