; Fill screen colors
	li	$1	144
a:	wb	$1	1792	$1	; 1792 is the offset for the background setup.
	dec	$1
	dec	$2
	jne	$2	$0	aa
	li	$2	10
	wrt
aa:	jne	$1	$0	a
; Draw hello world
	li	$1	0
	li	$2	0
	li	$3	768
	li	$4	0
hw:	lb 	$2 	dt	$1
	wb	$2	1024	$4	; 1024 is the offset for the display memory
	inc	$1
	inc	$4
	dec	$3
	jeq	$3	$0	out	; If screen is full, jump out
	jne	$2	$0	hw	; if not zero, continue
	li	$1	0
	jmp	hw
; Cycle foreground
out:	li	$3	0
	li	$4	200
rl:	inc	$3
	li	$1	96
b:	add	$2	$3	$1
	wb	$2	1936	$1
	dec	$1
	jne	$1	$0	b
	dec	$5
	jne	$5	$0	ww
	li	$5	5
	wrt
ww:	jne	$3	$4	rl
	hlt

dt:	sdata HELLO WORLD
	data 0
