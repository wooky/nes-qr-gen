; Startup code for cc65 and Shiru's NES library
; based on code by Groepaz/Hitmen <groepaz@gmx.net>, Ullrich von Bassewitz <uz@cc65.org>



	.export _exit,__STARTUP__:absolute=1
	.import initlib,push0,popa,popax,_main,zerobss,copydata

	; Linker generated symbols
	.import __RAM_START__   ,__RAM_SIZE__
	.import __ROM0_START__  ,__ROM0_SIZE__
	.import __STARTUP_LOAD__,__STARTUP_RUN__,__STARTUP_SIZE__
	.import	__CODE_LOAD__   ,__CODE_RUN__   ,__CODE_SIZE__
	.import	__RODATA_LOAD__ ,__RODATA_RUN__ ,__RODATA_SIZE__
	.import NES_MAPPER,NES_PRG_BANKS,NES_CHR_BANKS,NES_MIRRORING,NES_PRG_RAM_SHIFT_COUNT,NES_CHR_RAM_SHIFT_COUNT,NES_FAMILY_BASIC_KEYBOARD

	.include "zeropage.inc"


PPU_CTRL	=$2000
PPU_MASK	=$2001
PPU_STATUS	=$2002
PPU_OAM_ADDR	=$2003
PPU_OAM_DATA	=$2004
PPU_SCROLL	=$2005
PPU_ADDR	=$2006
PPU_DATA	=$2007
PPU_OAM_DMA	=$4014
PPU_FRAMECNT	=$4017
CTRL_PORT1	=$4016
CTRL_PORT2	=$4017

OAM_BUF		=$0200
PAL_BUF		=$01c0



.segment "ZEROPAGE"

NTSC_MODE: 		.res 1
FRAME_CNT1: 		.res 1
FRAME_CNT2: 		.res 1
VRAM_UPDATE: 		.res 1
NAME_UPD_ADR: 		.res 2
NAME_UPD_ENABLE: 	.res 1
PAL_UPDATE: 		.res 1
PAL_BG_PTR: 		.res 2
PAL_SPR_PTR: 		.res 2
PAD_STATE: 		.res 2		;one byte per controller
PAD_STATEP: 		.res 2
PAD_STATET: 		.res 2
PPU_CTRL_VAR:		.res 1
PPU_CTRL_VAR1:		.res 1
PPU_MASK_VAR: 		.res 1

TEMP: 			.res 11

PAD_BUF			=TEMP+1

PTR			=TEMP	;word
LEN			=TEMP+2	;word
SRC			=TEMP+7	;word
DST			=TEMP+9	;word



.segment "HEADER"

    .byte $4e,$45,$53,$1a
	.byte <NES_PRG_BANKS
	.byte <NES_CHR_BANKS
	.byte <NES_MIRRORING|((<NES_MAPPER&$0f)<<4)
	.byte (<NES_MAPPER&$f0)|$8
	.byte >NES_MAPPER
	.byte $00
	.byte <NES_PRG_RAM_SHIFT_COUNT
	.byte <NES_CHR_RAM_SHIFT_COUNT
	.byte $00
	.byte $00
	.byte $00
	.byte <NES_FAMILY_BASIC_KEYBOARD



.segment "STARTUP"

start:
_exit:

    sei
    ldx #$ff
    txs
    inx
    stx PPU_MASK
    stx PPU_CTRL		;no NMI

initPPU:

    bit PPU_STATUS
@1:
    bit PPU_STATUS
    bpl @1
@2:
    bit PPU_STATUS
    bpl @2

clearPalette:

	lda #$3f
	sta PPU_ADDR
	stx PPU_ADDR
	lda #$0f
	ldx #$20
@1:
	sta PPU_DATA
	dex
	bne @1

clearVRAM:

	txa
	ldy #$20
	sty PPU_ADDR
	sta PPU_ADDR
	ldy #$10
@1:
	sta PPU_DATA
	inx
	bne @1
	dey
	bne @1

clearRAM:

    txa
@1:
    sta $000,x
    sta $100,x
    sta $200,x
    sta $300,x
    sta $400,x
    sta $500,x
    sta $600,x
    sta $700,x
    inx
    bne @1

	lda #4
	jsr _pal_bright
	jsr _pal_clear
	jsr _oam_clear

    jsr	zerobss
	jsr	copydata

    lda #<(__RAM_START__+__RAM_SIZE__)
    sta	sp
    lda	#>(__RAM_START__+__RAM_SIZE__)
    sta	sp+1            ; Set argument stack ptr

	; jsr	initlib

	lda #%10000000
	sta <PPU_CTRL_VAR
	sta PPU_CTRL		;enable NMI
	lda #%00000110
	sta <PPU_MASK_VAR

waitSync3:
	lda <FRAME_CNT1
@1:
	cmp <FRAME_CNT1
	beq @1

detectNTSC:
	ldx #52				;blargg's code
	ldy #24
@1:
	dex
	bne @1
	dey
	bne @1

	jsr _ppu_off

	lda #0
	sta PPU_OAM_ADDR

	jmp _main			;no parameters

	; .include "display.sinc"

	.include "neslib.s"

.segment "VECTORS"

	.word nmi	;$fffa vblank nmi
	.word start	;$fffc reset
	.word irq	;$fffe irq / brk

.segment "CHARS"

	.incbin "ascii.chr"
