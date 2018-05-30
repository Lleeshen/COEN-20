            .syntax unified
            .cpu cortex-m4
            .text


            //float FloatPoly(float x, float a[], int32_t n) ;
            //Evaluate polynomial at x where a[] is the coefficients, uses float
            .global FloatPoly
            .thumb_func
            .align
zero:       .float 0
FloatPoly:
            //x in S0, &a in R0, n in R1, used as counter
            VLDR S1,zero //Store sum

addFloat:
            CBZ R1,prepareFloat //Done when i = 0
            SUB R1,R1,1 //i = n-1
            ADD R2,R0,R1,LSL 2 //R2 = &a[i]
            VLDR S2,[R2] //S2 = a[i]
            VMUL.F32 S1,S1,S0 //sum = sum * x
            VADD.F32 S1,S1,S2 //sum = sum + a[i]
            B addFloat

prepareFloat:
            VMOV S0,S1
            BX LR

            //Q16 FixedPoly(Q16 x, Q16 a[], int32_t n)
            //Evaluate polynomial at x where a[] is the coefficients, uses fixed-point real
            .global FixedPoly
            .thumb_func
            .align

FixedPoly:
            //x in R0, &a[0] in R1, n in R2 used as counter
            //R3 will be sum, R4 will store temporary products
            PUSH {R4}
            LDR R3,=0 //Initialize sum to 0
addFixed:
            CBZ R2,prepareFixed //Done when i=0
            SUB R2,R2,1 //i = i-1
            LDR R12, [R1,R2,LSL 2] //R12 = a[i]
            SMULL R3,R4,R3,R0 //sum = sum * x
            LSRS.N R3,R3,16
            ORR R3,R3,R4,LSL 16
            ADD R3,R3,R12 //sum = sum + a[i]
            B addFixed
prepareFixed:
            MOV R0, R3
            POP {R4}
            BX LR
            .end
