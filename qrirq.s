  .export _irq_enable, _irq_disable

.segment "ZEROPAGE"

NEXT_CHR_BANK: .res 1

.segment "CODE"

irq:
  pha
  txa
  pha

  ; Prepare to set first bank
  lda #%00000000
  sta $8000
  ldx NEXT_CHR_BANK

  ; Wait until a stupidly precise moment
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop

  ; Set first bank
  stx $8001

  ; Set second bank
  lda #%00000001
  sta $8000
  inx
  inx
  stx $8001

  ; Acknowledge interrupt
  sta $e000

  ; Check if we just handled the 3rd chunk
  txa
  cmp #$4a
  beq @lastChunk

  ; Set IRQ latch and re-enable interrupts
  lda #87 ; 11 tiles per chunk * 8 rows per tile - 1
  sta $c000
  sta $e001

  ; Setup next bank
  inx
  inx
  jmp @done

@lastChunk:
  ; Reset next bank to the first bank
  ldx #$40

@done:
  stx NEXT_CHR_BANK

  pla
  tax
  pla

  rti

_irq_enable:
  lda #$40
  sta NEXT_CHR_BANK
  lda #7 ; trust me bro
  sta IRQ_LATCH
  cli
  rts

_irq_disable:
  lda #0
  sta IRQ_LATCH
  sta $e000
  sei
  rts
