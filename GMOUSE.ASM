;
; Graphic mouse cursor (C) BlockHead aka M.Jerry (Guru)
;
; This program has been developed on those videoadapter and mouse
; driver which I had at work (S3 ViRGE and standard WinNT Microsoft
; Serial Mouse driver). The specs of those:
; Character set:
;   - 32 bytes per each character in 80x25 text mode,
;     first 16 bytes is a character image (1 byte per scan line),
;     other 16 bytes are not used
; Mouse driver:
;   - 8 points horizontally / 8 point vertically per each character
;     So horizontally the mouse pointer moves smoothly and vertically
;     it jumps over one scanline since the character has the height
;     of 16 points. It doesn't catch the eye, though.
;
; NB:
;   - it does not restore characters CHAR0..CHAR3
;
IDEAL
MODEL LARGE
P386
CODESEG

public  _InstallGraphicsMouse    ; init drv, save bk, show graph
public  _DeleteGraphicsMouse     ; hide graph, rest bk
public  _ShowGraphicCursor       ; if (flag++) save bk, show graph
public  _HideGraphicCursor       ; if (flag--) rest bk

CHARWIDTH  equ  1              ; in bytes
CHARHEIGHT equ  (16*CHARWIDTH) ; in bytes
CHARSTEP   equ  32             ; step between characters in char map
CHARSHIFT  equ  5              ; LOG (2) CHARSTEP

MASKSIZE   equ  16             ; cursor mask height (scanlines)

CHAR0   equ     0D7h ; 'Ú'
CHAR1   equ     008h ; '¿'
CHAR2   equ     0D8h ; 'À'
CHAR3   equ     00Eh ; 'Ù'
CHFF0   equ     (CHAR0*CHARSTEP)
CHFF1   equ     (CHAR1*CHARSTEP)
CHFF2   equ     (CHAR2*CHARSTEP)
CHFF3   equ     (CHAR3*CHARSTEP)

; -------------------------------------------------------------

; Desc: calculates ystep and text cursor position
; In:   CX=textx, DX=texty
; Out:  CX,DX=same, [cs:ystep], [cs:textoffs]
proc    CalcPosition
        shr     cx,3
        shr     dx,3
        push    es
        push    cx
        push    dx
        xor     ax,ax
        mov     es,ax
        mov     ax,[es:044Ah]   ;
        shl     ax,1            ;
        dec     ax              ;
        dec     ax              ;
        dec     ax              ; AX = columns*2 - 3
        mov     [cs:ystep],ax   ; ystep = AX
        inc     ax
        inc     ax
        inc     ax
        mul     dx              ;
        shl     cx,1
        add     ax,cx
        mov     [cs:textoffs],ax ; AX = text offset
        pop     dx
        pop     cx
        pop     es
        ret
endp
; Desc: saves background from textoffs into backgr[]
; In:   ---
; Out:  SI,DI destroyed
proc    ReadBackground
        push    ds
        push    es
        push    0B800h          ;
        pop     ds              ;
        push    cs              ;
        pop     es              ; DS=B800h, ES=data
        mov     si,[cs:textoffs]
        mov     di,offset backgr
        movsb
        inc     si
        movsb
        add     si,[cs:ystep]
        movsb
        inc     si
        movsb
        pop     es
        pop     ds
        ret
endp
; Desc: writes saved background from backgr[] into prevoffs
; In:   ---
; Out:  SI,DI destroyed
proc    WriteBackground
        push    ds
        push    es
        push    0B800h          ;
        pop     es              ;
        push    cs              ;
        pop     ds              ; DS=data, ES=B800
; restore old background
        mov     di,[prevoffs]
        mov     si,offset backgr
        movsb
        inc     di
        movsb
        add     di,[ystep]
        movsb
        inc     di
        movsb
        pop     es
        pop     ds
        ret
endp
; Desc: writes 4 symbols of "graphic" cursor into textoffs
; In:   ---
; Out:  SI,DI destroyed
proc    WriteCursor
        push    ds
        push    es
        push    cs              ;
        pop     ds              ;
        push    0B800h          ;
        pop     es              ; DS=data, ES=B800h
        mov     si,offset cursor_chars
        mov     di,[textoffs]
        movsb
        inc     di
        movsb
        add     di,[ystep]
        movsb
        inc     di
        movsb
        pop     es
        pop     ds
        ret
endp

; init drv, save bk, show graph
proc    _InstallGraphicsMouse   far

        mov     [cs:installed],0
; init mouse drv
        mov     ax,0
        int     33h
        inc     ax
        jnz     @@exit
        mov     [cs:installed],1
;        mov     ax,0Fh
;        mov     cx,32767
;        mov     dx,32767
;        int     33h
;        mov     ax,13h
;        mov     dx,10000
;        int     33h
@@ok:
; get mouse position
        mov     ax,3
        int     33h
        call    CalcPosition
        mov     ax,[cs:textoffs]
        mov     [cs:prevoffs],ax
; init gmouse
        mov     [cs:visible],0
; show graph
        call    _ShowGraphicCursor
; set user handler
        push    es
        mov     ax,cs
        mov     es,ax
        mov     dx,offset GraphicsMouse
        mov     cx,01h
        mov     ax,0Ch
        int     33h
        pop     es
@@exit: ret
endp

; hide graph
proc    _DeleteGraphicsMouse    far

        cmp     [cs:installed],1
        jne     @@exit
; hide graph
        call    _HideGraphicCursor
; remove user handler
        mov     ax,0Ch
        mov     cx,00h
        int     33h
@@exit: ret
endp

; save bk, flag++
proc    _ShowGraphicCursor       far
        push    ds
        push    es
        push    si
        push    di
        cmp     [cs:installed],1
        jne     @@exit
        mov     ax,[cs:visible]
        cmp     ax,1
        je      @@exit
        inc     ax
        mov     [cs:visible],ax
        cmp     ax,1
        jne     @@exit
        call    ReadBackground
        call    WriteCursor
        call    Hardware
@@exit: pop     di
        pop     si
        pop     es
        pop     ds
        ret
endp

; if (flag--) rest bk
proc    _HideGraphicCursor       far
        push    ds
        push    es
        push    si
        push    di
        cmp     [cs:installed],1
        jne     @@exit
        cmp     [cs:visible],1
        jne     @@exit
        call    WriteBackground
@@exit: pop     di
        pop     si
        pop     es
        pop     ds
        dec     [cs:visible]
        ret
endp

;   character map()
;     open character map
;     read base characters
;     calculate offsets
;     write charmap
;     close charmap

proc    Hardware
; open charmap
        mov     ax,0A000h       ;
        mov     ds,ax           ;
        mov     es,ax           ; DS=ES=video seg

        mov     dx,3C4h         ; sequencer
        mov     ax,0402h        ; AL=2, map mask
        out     dx,ax           ; AH=00000100, plane 2 (charset)
        mov     ax,0704h        ; AL=4, memory mode
        out     dx,ax           ; AH=00000111, D2=sequential
        mov     dl,0CEh         ; graphics registers
        mov     ax,0005h        ; AL=5, mode
        out     dx,ax           ; AH=0, serial output (mode 0)
        mov     ax,0406h        ; AL=6, misc
        out     dx,ax           ; AH=00000100, D2,D3=videomemory A0000-AFFFFh
        mov     ax,0204h        ; AL=4, read map select
        out     dx,ax           ; AH=2, plane 2

; debug :) :(
        mov     si,1*CHARSTEP
        mov     ah,[si+9]
        mov     al,[si+10]
        mov     [si+9],al
        mov     [si+10],ah

; read base characters
        push    cs                      ;
        pop     es                      ; DS=A000,ES=data
        mov     di,offset fbase
        xor     dx,dx
@@base: mov     bx,offset backgr
        add     bx,dx
        add     bx,dx
        mov     cl,[byte ptr es:bx]     ; load background char (1/2)
        mov     ch,0
        shl     cx,CHARSHIFT            ; CX*=N, offset in charmap
        mov     si,cx
        inc     bx
        mov     bl,[byte ptr es:bx]     ; load background char (2/2)
        mov     bh,0
        shl     bx,CHARSHIFT            ; BX*=N, offset in charmap
        mov     cx,CHARHEIGHT
@@read:         mov     ah,[byte ptr si]
                mov     al,[byte ptr bx]
                stosw
                inc     si
                inc     bx
                loop    @@read
        inc     dx
        cmp     dx,2
        jne     @@base

write_cursor:
        push    cs                      ;
        pop     ds                      ; DS=ES=data
; calculate offset
        mov     ax,[realx]
        mov     bx,[textx]
        shl     bx,3
        sub     ax,bx
        mov     [shiftx],ax
        mov     ax,[realy]
        mov     bx,[texty]
        shl     bx,3
        sub     ax,bx
        shl     ax,1
        mov     [shifty],ax
; shift mask
        mov     si,offset and_mask      ;
        mov     di,offset and_shift     ; DS=ES=data
        mov     dx,MASKSIZE*2           ; 2 masks, 2 characters per mask
        mov     cx,[shiftx]
@@shift:
        lodsw
        ror     ax,cl
        stosw
        dec     dx
        jnz     @@shift

; apply mask
        mov     si,offset fbase
        mov     di,offset fwrite
        xor     bx,bx           ; BX = cursor mask line counter
        xor     dx,dx           ; DX = character count (2 total)
@@chars:                        ; cursor character loop
        mov     cx,CHARHEIGHT   ; CX = character line counter
@@lines:                        ; character lines (bytes) loop
                lodsw
                cmp     [shifty],0
                jne     @@above
                cmp     bx,MASKSIZE*2
                je      @@below
                and     ax,[word ptr and_shift+bx]
                xor     ax,[word ptr and_shift+bx+MASKSIZE*2]
                inc     bx
                inc     bx
                jmp     @@below
        @@above:
                dec     [shifty]
        @@below:
                stosw
                loop    @@lines

        inc     dx
        cmp     dx,2
        jne     @@chars

; write charmap
        mov     ax,0A000h               ;
        mov     es,ax                   ; DS=data,ES=A000
        mov     si,offset fwrite
        xor     dx,dx                   ; DX = character pairs counter
@@char:
        mov     bx,dx
        shl     bx,2
        mov     di,[cursor_offset+bx]   ; 1st char offset
        inc     bx
        inc     bx
        mov     bx,[cursor_offset+bx]   ; 2nd char offset
        mov     cx,CHARHEIGHT
@@store:        lodsw
                mov     [byte ptr es:bx],al
                mov     [byte ptr es:di],ah
                inc     bx
                inc     di
                loop    @@store
        inc     dx
        cmp     dx,2
        jne     @@char

; close character map
        mov     dx,3C4h         ; sequencer
        mov     ax,0302h        ; AL=2, map mask
        out     dx,ax           ; AH=00000011, planes 0,1
        mov     al,04h          ; AL=4, memory mode
        out     dx,ax           ; AH=00000011, unlink planes
        mov     dl,0CEh         ; graphics registers
        mov     ax,1005h        ; AL=5, mode
        out     dx,ax           ; AH=00010000, mode 0, D4=odd/even
        mov     ax,0E06h        ; AL=6, misc
        out     dx,ax           ; AH=00001110, ext.mem, D2-D3=B8000-BFFFFh
        mov     ax,0004h        ; AL=4, read map select
        out     dx,ax           ; AH=0, plane 0

        ret
endp

; int 33, function 0Ch handler
; In:
;   AX = condition mask
;   BX = button state
;   CX = X position
;   DX = Y position
;
proc    GraphicsMouse   far
;
; and xor color (0=back, 1=text)
;  0   0   0
;  0   1   1
;  1   0   same
;  1   1   inv
;
        pushf
        push    ds
        push    es
        push    si
        push    di
; save graphics coords
        push    cs
        pop     ds
        mov     [realx],cx
        mov     [realy],dx
; if the cursor is off - quit
        cmp     [visible],0
        je      @@exit
; calc text coords
        call    CalcPosition
        mov     [textx],cx
        mov     [texty],dx
        call    WriteBackground ; write old background
        call    ReadBackground  ; read new background
        call    WriteCursor     ;
; store background offset
        mov     ax,[textoffs]
        mov     [prevoffs],ax
@@graph:
        call    Hardware        ; update charmap

@@exit: pop     di
        pop     si
        pop     es
        pop     ds
        popf

        retf

endp    GraphicsMouse

cursor_chars    db      CHAR0,CHAR1,CHAR2,CHAR3
cursor_offset   dw      CHFF0,CHFF1,CHFF2,CHFF3

and_mask        dw      03FFFh,01FFFh,00FFFh,007FFh
                dw      003FFh,001FFh,000FFh,000FFh
                dw      000FFh,001FFh,021FFh,0F1FFh
                dw      0F0FFh,0F0FFh,0F9FFh,0FFFFh
xor_mask        dw      00000h,04000h,06000h,07000h
                dw      07800h,07C00h,07E00h,07E00h
                dw      07800h,04C00h,00C00h,00400h
                dw      00600h,00600h,00000h,00000h

installed       db      0
visible         dw      1
realx           dw      ?
realy           dw      ?
textx           dw      ?
texty           dw      ?
prevx           dw      ?
prevy           dw      ?
textoffs        dw      ?
prevoffs        dw      ?
ystep           dw      ?
backgr          db      4 dup(?)

shiftx          dw      ?
shifty          dw      ?
fbase           db      4*16 dup (?)    ; 4 16-byte characters under the cursor
fwrite          db      4*16 dup (?)
and_shift       db      32 dup (?)
xor_shift       db      32 dup (?)

END