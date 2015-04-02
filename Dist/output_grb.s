	AREA    Out_grb, CODE, READONLY
	EXPORT 	output_grb

;R0 address of GpioPortB Data
;R1 pointer to END of LED data matrix (start: row 7, col 7, green value)
;		will remain in row 7 to keep start of each ouput bit
;		is decremented by one after each color is finished
;R2 Address that is minipulated up each row
;R3 Col count- starts at 24
;R4 Data to be written
;R5	the bit in the current byte in which we want to grab
;R6 Row count- starts at 8
;R7	wait counter- contains value to wait to hit timing
;R8	tmp reg
;R9  Minipulated to get desired bit
;R10 0x00
;R11 0xFF

	ENTRY
output_grb
	PUSH {R5-R12}

	;Must reset the LEDs to get them ready for the data
	MOV		R10, #0
	MOV		R11, #0xFF
	MOV		R12, #2

inits
	;Initializations
	MOV		R4,	#0	  ;data to be written starts at 0
	MOV		R5, #7	  ;start the bit count with the MSB
	MOV 	R6, #8	  ;start at row/pin 7... but branch when 0
	MOV		R3, #24	  ;start at col 23.... but DONE when it is 0 
	MOV		R2, R1	  ;R2 is the minipulated address from current
					  ;		rows "base" (row 7)

start_bit
	;Start driving the pins with a 0xFF
	STRB	R11, [R0]
;accumulate the data to be written
next_row	
	LDRB	R9, [R2]     		;get the color value
	LSR		R9, R9, R5 	 		;shift right to get desired bit in LSB
	UDIV 	R8, R9, R12      	;R8 := R7 / 2	  ~MOD
	MLS  	R9, R12, R8, R9	 	;R7 := (R7 – (R12 * R8))   ~MOD
	LSL		R4, R4, #1			;Make room for the new bit value
	ADD		R4, R4, R9
	;Done with that row
	SUB		R6, R6, #1
	SUB		R2, R2, #24			;adjust the address to the byte in the row above
	
   	CBNZ	R6, to_nxtRow	   	
	B		output_Data			;once we accumulated the data in R4 - output it
to_nxtRow
	B		next_row

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

output_Data
	MOV		R7, #382  ;Must finish the 40us at 0xFF
t1_wait
	SUB		R7, R7, #1	
	CBNZ	R7, to_t1
	B		t2
to_t1
	B		t1_wait

t2
	STRB	R4, [R0]  ;For next 40us the pin logic should be exactly what the data is
	MOV		R7, #400
t2_wait
    SUB		R7, R7, #1	
	CBNZ	R7, to_t2
	B		t3
to_t2
	B		t2_wait

t3
	STRB	R10, [R0]  ;For last 45us the pin logic should be 0x00
	MOV		R7, #450
t3_wait
    SUB		R7, R7, #1	
	CBNZ	R7, to_t3
	B		check_bit
to_t3
	B		t3_wait

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

check_bit
	CBNZ	R5, to_nextBit	;check the bit count + 1, if it's 0 check the column
	B		check_col
to_nextBit
	SUB		R5, R5, #1		 ;need the next bit in this col
	MOV		R2, R1			 ;keep the col base address
	MOV		R4, #0			 ;reset the data to be sent
	MOV		R6, #8			 ;reset the row count
	B		start_bit  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	 
check_col
	SUB		R3, R3, #1
	CBNZ	R3, to_nextCol
	B		over
to_nextCol
	SUB		R1, R1, #1		  ;decrement the col base address to get the next col vals
	MOV		R2, R1
	MOV		R4, #0		      ;reset the data to be sent
	MOV		R6, #8			  ;reset the row count
	MOV		R5, #7			  ;reset the bit to the MSB
	B		start_bit	


over
	POP {R5-R12}
	BX		LR
	END

