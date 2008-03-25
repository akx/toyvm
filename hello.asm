	li	$1	0
;	li	$3	1010b
a1:	lb 	$2 	dt	$1	; $2 = m[$1+(dt)]
	or	$2	$3
	out	1	$2		; output $2
	inc	$1
	jeq	$2	$0	a2	; if zero, jump out
	jmp	a1
a2:	hlt
dt:	sdata Hello, world.
	data 0
