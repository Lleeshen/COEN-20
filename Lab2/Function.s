            .syntax unified
            .cpu cortex-m4
            .text


            //Ten32 Function
            //Returns an unsigned 32 bit version of 10
            .global Ten32
            .thumb_func
            .align

Ten32:      //Store 10 in R0
            LDR R0,=10
            //Return
            BX LR

            //Ten64 Function
            //Returns an unsigned 64 bit version of 10
            //Return R4 and R5 registersal Ten64
            .global Ten64
            .thumb_func
            .align

Ten64:      //Store 10 in R0
            LDR R0,=10
            //Store 0 in R1 to Zero-extend
            LDR R1,=0
            //Return
            BX LR

            //Incr Function
            //Increases the parameter, an unsigned 32 bit integer by 1
            //Return the result
            .global Incr
            .thumb_func
            .align

Incr:       //Add 1 to the parameter
            //The value is already in return register
            ADD R0,R0,1
            //Return
            BX LR

            //Nested1 Function
            //Gets a random number and increments it by 1
            //Return the result
            .global Nested1
            .thumb_func
            .align

Nested1:    //Save link register so it is not lost
            PUSH {LR}
            //Call rand function
            BL rand
            //Add 1 to the result. It is already in return register
            ADD R0,R0,1
            //Return
            POP {PC}


            //Nested1 Function
            //Gets 2 random numbers and sum them
            //Ret//Return R4 and R5 registersurn the result
            .global Nested2
            .thumb_func
            .align

Nested2:    //Save R4 register to use as temp
            PUSH {R4,LR}
            //Call rand the first time
            BL rand
            //Save result to temp
            MOV R4, R0
            //Call rand the second time
            BL rand
            //Add the two results. They are already in return register.
            ADD R0,R0,R4
            //Return and restore R4 register
            POP {R4, PC}


            //PrintTwo Function
            //Print a number and that number plus 1
            .global PrintTwo
            .thumb_func
            .align

PrintTwo:   //Save R4 and R5 register as temp
            PUSH {R4,R5,LR}
            //Copy format and n to temp registers
            MOV R4, R0
            MOV R5, R1
            //Call printf the first time
            BL printf
            //Put format back to be the first parameter
            MOV R0, R4
            //Put n back to the second parameter
            MOV R1, R5
            //Add 1 to n
            ADD R1, R1, 1
            //Call printf
            BL printf
            //Return and restore registers
            POP {R4,R5,PC}

.end
