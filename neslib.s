;NES hardware-dependent functions by Shiru (shiru@mail.ru)
;with improvements by VEG
;Feel free to do anything you want with this code, consider it Public Domain


	.export _pal_all,_pal_bg,_pal_spr,_pal_col,_pal_clear
	.export _pal_bright,_pal_spr_bright,_pal_bg_bright
	.export _ppu_off,_ppu_on_all,_ppu_on_bg,_ppu_on_spr,_ppu_mask,_ppu_system
	.export _oam_clear,_oam_size,_oam_spr
	.export _ppu_wait_frame,_ppu_wait_nmi
	.export _bank_spr,_bank_bg
	.export _vram_read,_vram_write
	.export _pad_poll,_pad_trigger,_pad_state
	.export _vram_adr,_vram_put,_vram_fill,_vram_inc,_vram_unrle
	.export _set_vram_update,_flush_vram_update
	.export _memfill,_delay
	.export _get_ppu_ctrl_var,_set_ppu_ctrl_var
	.export _nesclock



;NMI handler

nmi:
	pha
	txa
	pha
	tya
	pha

	lda <PPU_MASK_VAR	;if rendering is disabled, do not access the VRAM at all
	and #%00011000
	bne @doUpdate
	jmp	@skipAll

@doUpdate:

	lda #>OAM_BUF		;update OAM
	sta PPU_OAM_DMA

	lda <PAL_UPDATE		;update palette if needed
	bne @updPal
	jmp @updVRAM

@updPal:

	ldx #0
	stx <PAL_UPDATE

	lda #$3f
	sta PPU_ADDR
	stx PPU_ADDR

	ldy PAL_BUF				;background color, remember it in X
	lda (PAL_BG_PTR),y
	sta PPU_DATA
	tax

	.repeat 3,I
	ldy PAL_BUF+1+I
	lda (PAL_BG_PTR),y
	sta PPU_DATA
	.endrepeat

	.repeat 3,J
	stx PPU_DATA			;background color
	.repeat 3,I
	ldy PAL_BUF+5+(J*4)+I
	lda (PAL_BG_PTR),y
	sta PPU_DATA
	.endrepeat
	.endrepeat

	.repeat 4,J
	stx PPU_DATA			;background color
	.repeat 3,I
	ldy PAL_BUF+17+(J*4)+I
	lda (PAL_SPR_PTR),y
	sta PPU_DATA
	.endrepeat
	.endrepeat

@updVRAM:

	lda <VRAM_UPDATE
	beq @skipUpd
	lda #0
	sta <VRAM_UPDATE

	lda <NAME_UPD_ENABLE
	beq @skipUpd

	jsr _flush_vram_update_nmi

@skipUpd:

	lda #0
	sta PPU_ADDR
	sta PPU_ADDR

	lda <PPU_CTRL_VAR
	sta PPU_CTRL

@skipAll:

	lda <IRQ_LATCH
	beq @skipIrq
	sta $c000
	sta $c001
	sta $e001

@skipIrq:

	lda <PPU_MASK_VAR
	sta PPU_MASK

	inc <FRAME_CNT1
	inc <FRAME_CNT2
	lda <FRAME_CNT2
	cmp #6
	bne @skipNtsc
	lda #0
	sta <FRAME_CNT2

@skipNtsc:

	pla
	tay
	pla
	tax
	pla

  rti



;void __fastcall__ pal_all(const char *data);

_pal_all:

	sta <PTR
	stx <PTR+1
	ldx #$00
	lda #$20

pal_copy:

	sta <LEN

	ldy #$00

@0:

	lda (PTR),y
	sta PAL_BUF,x
	inx
	iny
	dec <LEN
	bne @0

	inc <PAL_UPDATE

	rts



;void __fastcall__ pal_bg(const char *data);

_pal_bg:

	sta <PTR
	stx <PTR+1
	ldx #$00
	lda #$10
	bne pal_copy ;bra



;void __fastcall__ pal_spr(const char *data);

_pal_spr:

	sta <PTR
	stx <PTR+1
	ldx #$10
	txa
	bne pal_copy ;bra



;void __fastcall__ pal_col(unsigned char index,unsigned char color);

_pal_col:

	sta <PTR
	jsr popa
	and #$1f
	tax
	lda <PTR
	sta PAL_BUF,x
	inc <PAL_UPDATE
	rts



;void __fastcall__ pal_clear(void);

_pal_clear:

	lda #$0f
	ldx #0

@1:

	sta PAL_BUF,x
	inx
	cpx #$20
	bne @1
	stx <PAL_UPDATE
	rts



;void __fastcall__ pal_spr_bright(unsigned char bright);

_pal_spr_bright:

	tax
	lda palBrightTableL,x
	sta <PAL_SPR_PTR
	lda palBrightTableH,x	;MSB is never zero
	sta <PAL_SPR_PTR+1
	sta <PAL_UPDATE
	rts



;void __fastcall__ pal_bg_bright(unsigned char bright);

_pal_bg_bright:

	tax
	lda palBrightTableL,x
	sta <PAL_BG_PTR
	lda palBrightTableH,x	;MSB is never zero
	sta <PAL_BG_PTR+1
	sta <PAL_UPDATE
	rts



;void __fastcall__ pal_bright(unsigned char bright);

_pal_bright:

	jsr _pal_spr_bright
	txa
	jmp _pal_bg_bright



;void __fastcall__ ppu_off(void);

_ppu_off:

	lda <PPU_MASK_VAR
	and #%11100111
	sta <PPU_MASK_VAR
	jmp _ppu_wait_nmi



;void __fastcall__ ppu_on_all(void);

_ppu_on_all:

	lda <PPU_MASK_VAR
	ora #%00011000

ppu_onoff:

	sta <PPU_MASK_VAR
	jmp _ppu_wait_nmi



;void __fastcall__ ppu_on_bg(void);

_ppu_on_bg:

	lda <PPU_MASK_VAR
	ora #%00001000
	bne ppu_onoff	;bra



;void __fastcall__ ppu_on_spr(void);

_ppu_on_spr:

	lda <PPU_MASK_VAR
	ora #%00010000
	bne ppu_onoff	;bra



;void __fastcall__ ppu_mask(unsigned char mask);

_ppu_mask:

	sta <PPU_MASK_VAR
	rts



;unsigned char __fastcall__ ppu_system(void);

_ppu_system:

	lda <NTSC_MODE
	ldx #$00
	rts

;unsigned char __fastcall__ get_ppu_ctrl_var(void);

_get_ppu_ctrl_var:

	lda <PPU_CTRL_VAR
	ldx #$00
	rts

;void __fastcall__ set_ppu_ctrl_var(unsigned char var);

_set_ppu_ctrl_var:

	sta <PPU_CTRL_VAR
	rts



;void __fastcall__ oam_clear(void);

_oam_clear:

	ldx #0
	lda #$ff
@1:
	sta OAM_BUF,x
	inx
	inx
	inx
	inx
	bne @1
	rts



;void __fastcall__ oam_size(unsigned char size);

_oam_size:

	asl a
	asl a
	asl a
	asl a
	asl a
	and #$20
	sta <TEMP
	lda <PPU_CTRL_VAR
	and #$df
	ora <TEMP
	sta <PPU_CTRL_VAR

	rts



;unsigned char __fastcall__ oam_spr(unsigned char x,unsigned char y,unsigned char chrnum,unsigned char attr,unsigned char sprid);

_oam_spr:

	tax

	ldy #0		;four popa calls replacement
	lda (sp),y
	iny
	sta OAM_BUF+2,x
	lda (sp),y
	iny
	sta OAM_BUF+1,x
	lda (sp),y
	iny
	sta OAM_BUF+0,x
	lda (sp),y
	sta OAM_BUF+3,x

	lda <sp
	clc
	adc #4
	sta <sp
	bcc @1
	inc <sp+1

@1:

	txa
	clc
	adc #4
	ldx #$00
	rts



;void __fastcall__ ppu_wait_frame(void);

_ppu_wait_frame:

	lda #1
	sta <VRAM_UPDATE
	lda <FRAME_CNT1

@1:

	cmp <FRAME_CNT1
	beq @1
	lda <NTSC_MODE
	beq @3

@2:

	lda <FRAME_CNT2
	cmp #5
	beq @2

@3:

	rts



;void __fastcall__ ppu_wait_nmi(void);

_ppu_wait_nmi:

	lda #1
	sta <VRAM_UPDATE
	lda <FRAME_CNT1
@1:

	cmp <FRAME_CNT1
	beq @1
	rts



;void __fastcall__ vram_unrle(const unsigned char *data);

_vram_unrle:

	tay
	stx <RLE_HIGH
	lda #0
	sta <RLE_LOW

	lda (RLE_LOW),y
	sta <RLE_TAG
	iny
	bne @1
	inc <RLE_HIGH

@1:

	lda (RLE_LOW),y
	iny
	bne @11
	inc <RLE_HIGH

@11:

	cmp <RLE_TAG
	beq @2
	sta PPU_DATA
	sta <RLE_BYTE
	bne @1

@2:

	lda (RLE_LOW),y
	beq @4
	iny
	bne @21
	inc <RLE_HIGH

@21:

	tax
	lda <RLE_BYTE

@3:

	sta PPU_DATA
	dex
	bne @3
	beq @1

@4:

	rts



;void __fastcall__ bank_spr(unsigned char n);

_bank_spr:

	and #$01
	asl a
	asl a
	asl a
	sta <TEMP
	lda <PPU_CTRL_VAR
	and #%11110111
	ora <TEMP
	sta <PPU_CTRL_VAR

	rts



;void __fastcall__ bank_bg(unsigned char n);

_bank_bg:

	and #$01
	asl a
	asl a
	asl a
	asl a
	sta <TEMP
	lda <PPU_CTRL_VAR
	and #%11101111
	ora <TEMP
	sta <PPU_CTRL_VAR

	rts


;void __fastcall__ vram_read(unsigned char *dst,unsigned int size);

_vram_read:

	sta <TEMP
	stx <TEMP+1

	jsr popax
	sta <TEMP+2
	stx <TEMP+3

	lda PPU_DATA

	ldy #0

@1:

	lda PPU_DATA
	sta (TEMP+2),y
	inc <TEMP+2
	bne @2
	inc <TEMP+3

@2:

	lda <TEMP
	bne @3
	dec <TEMP+1

@3:

	dec <TEMP
	lda <TEMP
	ora <TEMP+1
	bne @1

	rts



;void __fastcall__ vram_write(unsigned char *src,unsigned int size);

_vram_write:

	sta <TEMP
	stx <TEMP+1

	jsr popax
	sta <TEMP+2
	stx <TEMP+3

	ldy #0

@1:

	lda (TEMP+2),y
	sta PPU_DATA
	inc <TEMP+2
	bne @2
	inc <TEMP+3

@2:

	lda <TEMP
	bne @3
	dec <TEMP+1

@3:

	dec <TEMP
	lda <TEMP
	ora <TEMP+1
	bne @1

	rts


;unsigned char __fastcall__ pad_poll(unsigned char pad);

_pad_poll:

	tay
	ldx #0

@padPollPort:

	lda #1
	sta CTRL_PORT1
	lda #0
	sta CTRL_PORT1
	lda #8
	sta <TEMP

@padPollLoop:

	lda CTRL_PORT1,y
	lsr a
	ror <PAD_BUF,x
	dec <TEMP
	bne @padPollLoop

	inx
	cpx #3
	bne @padPollPort

	lda <PAD_BUF
	cmp <PAD_BUF+1
	beq @done
	cmp <PAD_BUF+2
	beq @done
	lda <PAD_BUF+1

@done:

	sta <PAD_STATE,y
	tax
	eor <PAD_STATEP,y
	and <PAD_STATE ,y
	sta <PAD_STATET,y
	txa
	sta <PAD_STATEP,y

	ldx #$00
	rts



;unsigned char __fastcall__ pad_trigger(unsigned char pad);

_pad_trigger:

	pha
	jsr _pad_poll
	pla
	tax
	lda <PAD_STATET,x
	ldx #$00
	rts



;unsigned char __fastcall__ pad_state(unsigned char pad);

_pad_state:

	tax
	lda <PAD_STATE,x
	ldx #$00
	rts



;void __fastcall__ set_vram_update(unsigned char *buf);

_set_vram_update:

	sta <NAME_UPD_ADR+0
	stx <NAME_UPD_ADR+1
	ora <NAME_UPD_ADR+1
	sta <NAME_UPD_ENABLE

	rts



;void __fastcall__ flush_vram_update(unsigned char *buf);

_flush_vram_update:

	sta <NAME_UPD_ADR+0
	stx <NAME_UPD_ADR+1

_flush_vram_update_nmi:

	ldy #0

@updName:

	lda (NAME_UPD_ADR),y
	iny
	cmp #$40				;is it a non-sequental write?
	bcs @updNotSeq
	sta PPU_ADDR
	lda (NAME_UPD_ADR),y
	iny
	sta PPU_ADDR
	lda (NAME_UPD_ADR),y
	iny
	sta PPU_DATA
	jmp @updName

@updNotSeq:

	tax
	lda <PPU_CTRL_VAR
	cpx #$80				;is it a horizontal or vertical sequence?
	bcc @updHorzSeq
	cpx #$ff				;is it end of the update?
	beq @updDone

@updVertSeq:

	ora #$04
	bne @updNameSeq			;bra

@updHorzSeq:

	and #$fb

@updNameSeq:

	sta PPU_CTRL

	txa
	and #$3f
	sta PPU_ADDR
	lda (NAME_UPD_ADR),y
	iny
	sta PPU_ADDR
	lda (NAME_UPD_ADR),y
	iny
	tax

@updNameLoop:

	lda (NAME_UPD_ADR),y
	iny
	sta PPU_DATA
	dex
	bne @updNameLoop

	lda <PPU_CTRL_VAR
	sta PPU_CTRL

	jmp @updName

@updDone:

	rts



;void __fastcall__ vram_adr(unsigned int adr);

_vram_adr:

	stx PPU_ADDR
	sta PPU_ADDR

	rts



;void __fastcall__ vram_put(unsigned char n);

_vram_put:

	sta PPU_DATA

	rts



;void __fastcall__ vram_fill(unsigned char n,unsigned int len);

_vram_fill:

	sta <LEN
	stx <LEN+1
	jsr popa
	ldx <LEN+1
	beq @2
	ldx #0

@1:

	sta PPU_DATA
	dex
	bne @1
	dec <LEN+1
	bne @1

@2:

	ldx <LEN
	beq @4

@3:

	sta PPU_DATA
	dex
	bne @3

@4:

	rts



;void __fastcall__ vram_inc(unsigned char n);

_vram_inc:

	ora #0
	beq @1
	lda #$04

@1:

	sta <TEMP
	lda <PPU_CTRL_VAR
	and #$fb
	ora <TEMP
	sta <PPU_CTRL_VAR
	sta PPU_CTRL

	rts



;void __fastcall__ memfill(void *dst,unsigned char value,unsigned int len);

_memfill:

	sta <LEN
	stx <LEN+1
	jsr popa
	sta <TEMP
	jsr popax
	sta <DST
	stx <DST+1

	ldx #0

@1:

	lda <LEN+1
	beq @2
	jsr @3
	dec <LEN+1
	inc <DST+1
	jmp @1

@2:

	ldx <LEN
	beq @5

@3:

	ldy #0
	lda <TEMP

@4:

	sta (DST),y
	iny
	dex
	bne @4

@5:

	rts


;unsigned char __fastcall__ nesclock(void);

_nesclock:
	lda <FRAME_CNT1
	ldx #$00
	rts

;void __fastcall__ delay(unsigned char frames);

_delay:

	tax

@1:

	jsr _ppu_wait_nmi
	dex
	bne @1

	rts



palBrightTableL:

	.byte <palBrightTable0,<palBrightTable1,<palBrightTable2
	.byte <palBrightTable3,<palBrightTable4,<palBrightTable5
	.byte <palBrightTable6,<palBrightTable7,<palBrightTable8

palBrightTableH:

	.byte >palBrightTable0,>palBrightTable1,>palBrightTable2
	.byte >palBrightTable3,>palBrightTable4,>palBrightTable5
	.byte >palBrightTable6,>palBrightTable7,>palBrightTable8

palBrightTable0:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f	;black
palBrightTable1:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
palBrightTable2:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
palBrightTable3:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
palBrightTable4:
	.byte $00,$01,$02,$03,$04,$05,$06,$07,$08,$09,$0a,$0b,$0c,$0f,$0f,$0f	;normal colors
palBrightTable5:
	.byte $10,$11,$12,$13,$14,$15,$16,$17,$18,$19,$1a,$1b,$1c,$00,$00,$00
palBrightTable6:
	.byte $10,$21,$22,$23,$24,$25,$26,$27,$28,$29,$2a,$2b,$2c,$10,$10,$10	;$10 because $20 is the same as $30
palBrightTable7:
	.byte $30,$31,$32,$33,$34,$35,$36,$37,$38,$39,$3a,$3b,$3c,$20,$20,$20
palBrightTable8:
	.byte $30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30	;white
	.byte $30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30
	.byte $30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30
	.byte $30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30
