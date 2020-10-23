                MVI     R6, 32767
l9:             MVI     R4, 0           ; Function main
                STOR    M[R6], R4
                DEC     R6
l10:            MVI     R4, 1           ; Evaluate while condition
                TEST    R4, R4
                JMP.Z   l12
                                        ; Begin while
                MVI     R4, 88          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 58          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 32          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                JAL     l8              ; Call 'get_input'
                MOV     R4, R3
                STOR    M[R6], R4
                DEC     R6
                                        ; Call 'new_line'
                MVI     R4, 2           ; Get var 'line_it' address
                ADD     R4, R4, R6
                STOR    M[R6], R4
                DEC     R6
                JAL     l5
                MOV     R0, R3
                MVI     R4, 89          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 58          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 32          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                JAL     l8              ; Call 'get_input'
                MOV     R4, R3
                STOR    M[R6], R4
                DEC     R6
                                        ; Call 'new_line'
                MVI     R4, 3           ; Get var 'line_it' address
                ADD     R4, R4, R6
                STOR    M[R6], R4
                DEC     R6
                JAL     l5
                MOV     R0, R3
                MVI     R4, 77          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 97          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 120         ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 40          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 88          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 44          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 32          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 89          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 41          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 58          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 32          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                                        ; Call 'write_digit'
                                        ; Call 'max'
                MVI     R4, 2           ; Get var 'x' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R4, 2           ; Get var 'y' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                JAL     l3
                STOR    M[R6], R3
                DEC     R6
                JAL     l7
                MOV     R0, R3
                                        ; Call 'new_line'
                MVI     R4, 3           ; Get var 'line_it' address
                ADD     R4, R4, R6
                STOR    M[R6], R4
                DEC     R6
                JAL     l5
                MOV     R0, R3
                MVI     R4, 2           ; Pop stack frame
                ADD     R6, R6, R4
                JMP     l10             ; End while
l12:            BR      l12             ; Halt program
                                        ; Function get_input
l8:             STOR    M[R6], R7       ; Push return address to stack
                DEC     R6
l13:            MVI     R4, 65533       ; Evaluate while condition
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R5, 0
                INC     R6
                LOAD    R4, M[R6]
                SUB     R4, R4, R5
                BR.Z    5
                MVI     R4, 0
                BR      3
                MVI     R4, 1
                TEST    R4, R4
                JMP.Z   l14
                                        ; Begin while
                JMP     l13             ; End while
l14:            MVI     R4, 65535
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                                        ; Call 'write_chr'
                MVI     R4, 1           ; Get var 'c' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                MVI     R4, 1           ; Get var 'c' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R5, 48
                INC     R6
                LOAD    R4, M[R6]
                SUB     R3, R4, R5
                MVI     R4, 2           ; Return
                ADD     R6, R6, R4
                LOAD    R4, M[R6]
                JMP     R4
                                        ; Function write_digit
l7:             STOR    M[R6], R7       ; Push return address to stack
                DEC     R6
                MVI     R4, 48          ; Call 'write_chr'
                STOR    M[R6], R4
                DEC     R6
                MVI     R4, 3           ; Get var 'x' address
                ADD     R4, R4, R6
                LOAD    R5, M[R4]
                INC     R6
                LOAD    R4, M[R6]
                ADD     R4, R4, R5
                STOR    M[R6], R4
                DEC     R6
                JAL     l6
                MOV     R0, R3
                INC     R6              ; Return
                LOAD    R4, M[R6]
                INC     R6
                JMP     R4
                                        ; Function write_chr
l6:             STOR    M[R6], R7       ; Push return address to stack
                DEC     R6
                MVI     R4, 2           ; Get var 'chr' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R4, 65534
                INC     R6
                LOAD    R5, M[R6]
                STOR    M[R4], R5
                INC     R6              ; Return
                LOAD    R4, M[R6]
                INC     R6
                JMP     R4
                                        ; Function new_line
l5:             STOR    M[R6], R7       ; Push return address to stack
                DEC     R6
                MVI     R4, 2           ; Get var 'line_it' address
                ADD     R4, R4, R6
                LOAD    R5, M[R4]
                LOAD    R4, M[R5]
                INC     R4
                STOR    M[R5], R4
                                        ; Evalute if condition
                MVI     R4, 2           ; Get var 'line_it' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R5, 44
                INC     R6
                LOAD    R4, M[R6]
                SUB     R4, R4, R5
                BR.P    5
                MVI     R4, 0
                BR      3
                MVI     R4, 1
                TEST    R4, R4
                JMP.Z   l15             ; Begin if
                JAL     l4              ; Call 'clear'
                MOV     R0, R3
                MVI     R4, 0
                STOR    M[R6], R4
                DEC     R6
                MVI     R4, 3           ; Get var 'line_it' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                INC     R6
                LOAD    R5, M[R6]
                STOR    M[R4], R5       ; End if
                                        ; Call 'shl_n'
l15:            MVI     R4, 2           ; Get var 'line_it' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R4, 8
                STOR    M[R6], R4
                DEC     R6
                JAL     l2
                STOR    M[R6], R3
                DEC     R6
                MVI     R4, 65532
                INC     R6
                LOAD    R5, M[R6]
                STOR    M[R4], R5
                INC     R6              ; Return
                LOAD    R4, M[R6]
                INC     R6
                JMP     R4
                                        ; Function clear
l4:             STOR    M[R6], R7       ; Push return address to stack
                DEC     R6
                MVI     R4, 0
                STOR    M[R6], R4
                DEC     R6
                                        ; Evaluate while condition
l16:            MVI     R4, 1           ; Get var 'y' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R5, 44
                INC     R6
                LOAD    R4, M[R6]
                SUB     R4, R4, R5
                BR.NP   5
                MVI     R4, 0
                BR      3
                MVI     R4, 1
                TEST    R4, R4
                JMP.Z   l17
                                        ; Begin while
                                        ; Call 'shl_n'
                MVI     R4, 1           ; Get var 'y' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R4, 8
                STOR    M[R6], R4
                DEC     R6
                JAL     l2
                STOR    M[R6], R3
                DEC     R6
                MVI     R4, 65532
                INC     R6
                LOAD    R5, M[R6]
                STOR    M[R4], R5
                MVI     R4, 0
                STOR    M[R6], R4
                DEC     R6
                                        ; Evaluate while condition
l18:            MVI     R4, 1           ; Get var 'x' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R5, 79
                INC     R6
                LOAD    R4, M[R6]
                SUB     R4, R4, R5
                BR.NP   5
                MVI     R4, 0
                BR      3
                MVI     R4, 1
                TEST    R4, R4
                JMP.Z   l19
                MVI     R4, 32          ; Begin while
                STOR    M[R6], R4
                DEC     R6
                MVI     R4, 65534
                INC     R6
                LOAD    R5, M[R6]
                STOR    M[R4], R5
                MVI     R5, 1           ; Get var 'x' address
                ADD     R5, R5, R6
                LOAD    R4, M[R5]
                INC     R4
                STOR    M[R5], R4
                JMP     l18             ; End while
l19:            MVI     R5, 2           ; Get var 'y' address
                ADD     R5, R5, R6
                LOAD    R4, M[R5]
                INC     R4
                STOR    M[R5], R4
                MVI     R4, 1           ; Pop stack frame
                ADD     R6, R6, R4
                JMP     l16             ; End while
l17:            MVI     R4, 2           ; Return
                ADD     R6, R6, R4
                LOAD    R4, M[R6]
                JMP     R4
                                        ; Function max
l3:             STOR    M[R6], R7       ; Push return address to stack
                DEC     R6
                                        ; Evalute if condition
                MVI     R4, 3           ; Get var 'x' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R4, 3           ; Get var 'y' address
                ADD     R4, R4, R6
                LOAD    R5, M[R4]
                INC     R6
                LOAD    R4, M[R6]
                SUB     R4, R4, R5
                BR.NN   5
                MVI     R4, 0
                BR      3
                MVI     R4, 1
                TEST    R4, R4
                JMP.Z   l20             ; Begin if
                MVI     R4, 3           ; Get var 'x' address
                ADD     R4, R4, R6
                LOAD    R3, M[R4]
                INC     R6              ; Return
                LOAD    R4, M[R6]
                MVI     R5, 2
                ADD     R6, R6, R5
                JMP     R4
                                        ; Begin else
l20:            MVI     R4, 2           ; Get var 'y' address
                ADD     R4, R4, R6
                LOAD    R3, M[R4]
                INC     R6              ; Return
                LOAD    R4, M[R6]
                MVI     R5, 2
                ADD     R6, R6, R5
                JMP     R4              ; End if
                                        ; Function shl_n
l2:             STOR    M[R6], R7       ; Push return address to stack
                DEC     R6
                                        ; Evaluate while condition
l21:            MVI     R4, 2           ; Get var 'y' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R5, 0
                INC     R6
                LOAD    R4, M[R6]
                SUB     R4, R4, R5
                BR.P    5
                MVI     R4, 0
                BR      3
                MVI     R4, 1
                TEST    R4, R4
                JMP.Z   l22
                                        ; Begin while
                MVI     R5, 2           ; Get var 'y' address
                ADD     R5, R5, R6
                LOAD    R4, M[R5]
                DEC     R4
                STOR    M[R5], R4
                MVI     R4, 3           ; Get var 'x' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                SHL     R4
                STOR    M[R6], R4
                DEC     R6
                MVI     R4, 4           ; Get var 'x' address
                ADD     R4, R4, R6
                INC     R6
                LOAD    R5, M[R6]
                STOR    M[R4], R5
                JMP     l21             ; End while
l22:            MVI     R4, 3           ; Get var 'x' address
                ADD     R4, R4, R6
                LOAD    R3, M[R4]
                INC     R6              ; Return
                LOAD    R4, M[R6]
                MVI     R5, 2
                ADD     R6, R6, R5
                JMP     R4
                                        ; Function mul
l1:             STOR    M[R6], R7       ; Push return address to stack
                DEC     R6
                                        ; Evalute if condition
                MVI     R4, 2           ; Get var 'y' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R5, 0
                INC     R6
                LOAD    R4, M[R6]
                SUB     R4, R4, R5
                BR.Z    5
                MVI     R4, 0
                BR      3
                MVI     R4, 1
                TEST    R4, R4
                JMP.Z   l23             ; Begin if
                MVI     R3, 0
                INC     R6              ; Return
                LOAD    R4, M[R6]
                MVI     R5, 2
                ADD     R6, R6, R5
                JMP     R4              ; End if
                                        ; Evalute if condition
l23:            MVI     R4, 2           ; Get var 'y' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R5, 1
                INC     R6
                LOAD    R4, M[R6]
                AND     R4, R4, R5
                STOR    M[R6], R4
                DEC     R6
                MVI     R5, 0
                INC     R6
                LOAD    R4, M[R6]
                SUB     R4, R4, R5
                BR.Z    5
                MVI     R4, 0
                BR      3
                MVI     R4, 1
                TEST    R4, R4
                JMP.Z   l24             ; Begin if
                                        ; Call 'mul'
                MVI     R4, 3           ; Get var 'x' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R4, 3           ; Get var 'y' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                SHR     R4
                STOR    M[R6], R4
                DEC     R6
                JAL     l1
                SHL     R3
                INC     R6              ; Return
                LOAD    R4, M[R6]
                MVI     R5, 2
                ADD     R6, R6, R5
                JMP     R4
                                        ; Begin else
l24:            MVI     R4, 3           ; Get var 'x' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                                        ; Call 'mul'
                MVI     R4, 4           ; Get var 'x' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R4, 4           ; Get var 'y' address
                ADD     R4, R4, R6
                LOAD    R4, M[R4]
                STOR    M[R6], R4
                DEC     R6
                MVI     R5, 1
                INC     R6
                LOAD    R4, M[R6]
                SUB     R4, R4, R5
                STOR    M[R6], R4
                DEC     R6
                JAL     l1
                MOV     R5, R3
                INC     R6
                LOAD    R4, M[R6]
                ADD     R3, R4, R5
                INC     R6              ; Return
                LOAD    R4, M[R6]
                MVI     R5, 2
                ADD     R6, R6, R5
                JMP     R4              ; End if
