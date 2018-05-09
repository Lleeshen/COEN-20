            .syntax unified
            .cpu cortex-m4
            .text


            //void DeleteItem(int32_t data[], int32_t items, int32_t index)
            //Deletes item from index and shifts elements so order is preserved
            .global DeleteItem
            .thumb_func
            .align

DeleteItem:
            //R0 = &data[0], R1 = items, R2 = index
            ADD R0, R0, R2, LSL 2 //R0 = data+index
            SUB R1, R1, 1         //R1 = items - 1
StartDelete:
            CMP R2,R1
            BGE  FinishDelete     //Loop if index < items - 1
            LDR R3, [R0,4]        // R3 = *(data + index + 1)
            STR R3, [R0]          // *(data+index) = R3
            ADD R2, R2, 1         // index = index + 1
            ADD R0, R0, 4         // data+index is updated
            B StartDelete
FinishDelete:
            BX LR

            //void InsertItem(int32_t data[], int32_t items, int32_t index, int32_t value)

            //Inserts value at index and shifts elements so order is preserved
            //Note: The item at the end of data is lost.
            .global InsertItem
            .thumb_func
            .align

InsertItem:
            //R0 = &data[0], R1 = items, R2 = index, R3 = value
            ADD R0, R0, R1, LSL 2
            SUB R0, R0, 4           //  R0 = data+items-1
            SUB R1, R1, 1           //  items = items - 1
StartInsertShift:
            CMP R1, R2
            BLE FinishInsertShift   // Loop if items > index
            LDR R12, [R0,-4]        // R3 = *(data + items - 1)
            STR R12, [R0]           // *(data + items) = *(data + items - 1)
            SUB R1, R1, 1           // items = items - 1
            SUB R0, R0, 4           // data + items is updated
            B StartInsertShift
FinishInsertShift:                  //R0 = data[index]
            STR R3, [R0]
            BX LR
            .end
