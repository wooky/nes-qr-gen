  .export _irq_enable, _irq_disable

.segment "ZEROPAGE"

IRQ_STAGE: .res 1

.segment "CODE"

irq:
  pha
  txa
  pha

  ; Acknowledge interrupt
  sta $e000

  ; Bootleg jump table
  ldx IRQ_STAGE
  beq @stage0
  dex
  beq @stage1

@stage2: ; Render chunk 3
  ; Set up correct banks
  ldx #%00000000
  stx $8000
  stx IRQ_STAGE
  lda #$48
  sta $8001
  ldx #%00000001
  stx $8000
  lda #$4a
  sta $8001

  ; Do not restart re-enable interrupts for this frame

  jmp @done

@stage0: ; Render chunk 1
  ; Set up correct banks
  stx $8000
  lda #$40
  sta $8001
  ldx #%00000001
  stx $8000
  stx IRQ_STAGE
  lda #$42
  sta $8001

  ; Set IRQ latch and re-enable interrupts
  lda #87 ; 11 tiles per chunk * 8 rows per tile - 1
  sta $c000
  sta $e001

  jmp @done

@stage1: ; Render chunk 2
  ; Set up correct banks
  stx $8000
  lda #$44
  sta $8001
  ldx #%00000001
  stx $8000
  lda #$46
  sta $8001

  ; Set IRQ latch and re-enable interrupts
  lda #87 ; 11 tiles per chunk * 8 rows per tile - 1
  sta $c000
  sta $e001
  lda #2
  sta IRQ_STAGE

@done:
  pla
  tax
  pla

  rti

_irq_enable:
  lda #0
  sta IRQ_STAGE
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
