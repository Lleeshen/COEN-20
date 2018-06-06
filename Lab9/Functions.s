            .syntax unified
            .cpu cortex-m4
            .text


            //void SIMD_USatAdd(uint8_t bytes [], uint32_t count, uint8_t amount) ;
            //Increase the intensity of RGB pixels, 40 bytes at a time
            .global SIMD_USatAdd
            .thumb_func
            .align
 SIMD_USatAdd:
            //R0 = &bytes[0], R1 = count, R2 = amount
            PUSH {R4-R11}
            //Copy the amount to other parts of the byte so can add multiple bytes at same time
            BFI R2,R2,8,8
            BFI R2,R2,16,16
            //Main loop adds the amount 40 bytes at the time until there is less than 40 bytes left
addMainLoop:
            CMP R1,40
            BLT addCleanLoop
            //Move the byte array, add amounts, and put back to the array
            LDMIA R0,{R3-R12}
            UQADD8 R3,R3,R2
            UQADD8 R4,R4,R2
            UQADD8 R5,R5,R2
            UQADD8 R6,R6,R2
            UQADD8 R7,R7,R2
            UQADD8 R8,R8,R2
            UQADD8 R9,R9,R2
            UQADD8 R10,R10,R2
            UQADD8 R11,R11,R2
            UQADD8 R12,R12,R2
            STMIA R0!,{R3-R12}
            SUB R1,R1,40
            B addMainLoop
            //Adds the amount 4 bytes at a time until all pixels have amount added
addCleanLoop:
            CBZ R1, addDone
            //Load from byte array, add amount, and put back to the array
            LDR R3,[R0]
            UQADD8 R3,R3,R2
            STR R3,[R0],4
            SUB R1,R1,4
            B addCleanLoop
addDone:
            POP {R4-R11}
            BX LR

            //void SIMD_USatSub(uint8_t bytes [], uint32_t count, uint8_t amount) ;
            //Decrease the intensity of RGB pixels, 40 bytes at a time
            .global SIMD_USatSub
            .thumb_func
            .align
SIMD_USatSub:
            PUSH {R4-R11}
            //Copy the amount to other parts of the byte so can subtract multiple bytes at same time
            BFI R2,R2,8,8
            BFI R2,R2,16,16
            //Main loop subtracts the amount 40 bytes at the time until there is less than 40 bytes left
subMainLoop:
            CMP R1,40
            BLT subCleanLoop
            //Move the byte array, subtract amounts, and put back to the array
            LDMIA R0,{R3-R12}
            UQSUB8 R3,R3,R2
            UQSUB8 R4,R4,R2
            UQSUB8 R5,R5,R2
            UQSUB8 R6,R6,R2
            UQSUB8 R7,R7,R2
            UQSUB8 R8,R8,R2
            UQSUB8 R9,R9,R2
            UQSUB8 R10,R10,R2
            UQSUB8 R11,R11,R2
            UQSUB8 R12,R12,R2
            STMIA R0!,{R3-R12}
            SUB R1,R1,40
            B subMainLoop
            //Subtracts the amount 4 bytes at a time until all pixels have amount added
subCleanLoop:
            CBZ R1, subDone
            //Load from byte array, subtract amount, and put back to the array
            LDR R3,[R0]
            UQSUB8 R3,R3,R2
            STR R3,[R0],4
            SUB R1,R1,4
            B subCleanLoop
subDone:
            POP {R4-R11}
            BX LR
            .end
