  
PAGE  59,132
  
;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;лл								         лл
;лл			        ACENT				         лл
;лл								         лл
;лл      Created:   21-Jan-92					         лл
;лл      Passes:    5	       Analysis Flags on: H		         лл
;лл								         лл
;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
  
.286c
  
data_1e		equ	1Ah			; (0040:001A=20h)
data_2e		equ	1Ch			; (0040:001C=20h)
data_3e		equ	80h			; (0040:0080=1Eh)
data_4e		equ	82h			; (0040:0082=3Eh)
  
seg_a		segment
		assume	cs:seg_a, ds:seg_a
  
  
		org	100h
  
acent		proc	far
  
start:
		jmp	InstalarTSR			; (0236)

; 0x4453 es la firma del TSR y la usa para testear si ya esta instalado

Signature	db	54h, 44h
		db      4Ch
ViejoVector9	dd	00000h
Activo		db	1
  
acent		endp
  
;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;
;			External Entry Point
;
;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
  
int_09h_entry	proc	far
		sti				; Enable interrupts
		push	ax
		push	bx
		push	cx
		push	dx
		push	di
		push	si
		push	ds
		push	es

		mov	ax,40h
		mov	es,ax
		mov	ax,17h
		mov	di,ax
		mov	dh,es:[di]
		mov	dl,dh            ; DH tiene todos los estados
		and	dl,04h           ; DL solo el de Ctrl

		in	al,60h           ; Leer el scan

                test    al,80h           ; Si es soltar una tecla o un codigo
                jne     LlamarViejoISR   ; extendido dejarlo pasar

                pushf                    ; Si no primero llamar al BIOS
                call	cs:ViejoVector9  ; y luego analizarla

                ; Si es la tecla MACRO mandarla
                cmp     al,111
                je      Traducir

                ; Si no es CTRL+XX salir
                or      dl,dl
                jz      SalirNormal

                ; Ver si esta en la lista de las que hay que traducir
                mov     cx,15
                mov     si,offset TablaTradu
Proximo:
                cmp     al,cs:[si]
                je      Traducir
                inc     si
                loop    Proximo
                jmp     short SalirNormal

                ; Mandar la tecla al buffer del teclado
Traducir:
                mov     ah,5
                mov     cl,0
                mov     ch,al
                int     16h

SalirNormal:
		cli				; Disable interrupts
		pop	es
		pop	ds
		pop	si
		pop	di
		pop	dx
		pop	cx
		pop	bx
		pop	ax
		iret				; Interrupt return
int_09h_entry	endp
  
LlamarViejoISR:
		pop	es
		pop	ds
		pop	si
		pop	di
		pop	dx
		pop	cx
		pop	bx
		pop	ax
		jmp	cs:ViejoVector9		; (64BF:0106=0)

TablaTradu      db 29h,2,4,5,6,8,9,0Ah,0Bh,0Dh,27h,28h,33h,34h,35h

InstalarTSR:
		push	ds
		push	es
		push	cs
		mov	ah,9
		mov	dx,offset CopyRight	; (64BF:0293=41h)
		int	21h			; DOS Services  ah=function 09h
						;  display char string at ds:dx
		pop	ds
		mov	ah,35h			; '5'
		mov	al,9
		int	21h			; DOS Services  ah=function 35h
						;  get intrpt vector al in es:bx
		mov	dx,es
		cmp	bx,offset int_09h_entry
		jne	NoEstaInst			; Jump if not equal
		mov	ax,offset Signature
		mov	di,ax
		mov	ax,es:[di]
		cmp	ax,4454h
		je	YaEstaInst			; Jump if equal
NoEstaInst:
		mov	word ptr cs:ViejoVector9,bx	; (64BF:0106=0)
		mov	word ptr cs:ViejoVector9+2,dx	; (64BF:0108=0)
		mov	dx,offset int_09h_entry
		mov	ah,25h			; '%'
		mov	al,9
		int	21h			; DOS Services  ah=function 25h
						;  set intrpt vector al to ds:dx
		mov	ah,9
		mov	dx,offset Instalado	; (64BF:02D6=69h)
		int	21h			; DOS Services  ah=function 09h
						;  display char string at ds:dx
		mov	ah,9
		mov	dx,offset TextoAyuda	; (64BF:0316=0Dh)
		int	21h			; DOS Services  ah=function 09h
						;  display char string at ds:dx
		pop	es
		pop	ds

                ; Liberar el entorno
                mov     ax,es:[002Ch]
                mov     es,ax
                mov     ah,49h
                int     21h

		mov	ah,31h			; '1'
		mov	dx,offset InstalarTSR
                shr     dx,4
                inc     dx
		int	21h			; DOS Services  ah=function 31h
						;  terminate & stay resident
YaEstaInst:
		mov	ah,9
		mov	dx,offset YaInstalado	; (64BF:02ED=65h)
		int	21h			; DOS Services  ah=function 09h
						;  display char string at ds:dx
		pop	es
		pop	ds
		mov	ah,0
		int	21h			; DOS Services  ah=function 00h
						;  terminate, cs=progm seg prefx

CopyRight	db	'KExtend 1.0 - BIOS keyboard enhancer for US '
                db      'keyboards by SET (c) 1997',13,10,'$'

Instalado	db	'installed resident.',13,10, '$'

YaInstalado	db	'is allready in memory.',13,10

TextoAyuda	db      13,10
		db	'   Reports Ctrl+`1457890=;,./ and the MACRO key'
		db	13,10, '$'
  
seg_a		ends
  
  
  
		end	start
