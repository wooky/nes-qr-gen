#if !defined(MMC3_H_)
#define MMC3_H_

#define mcc3_prg_bank(section_8kb, bank_number) { \
  *(unsigned char*)0x8000 = 0b00000110 | section_8kb; \
  *(unsigned char*)0x8001 = bank_number; \
}

#define mmc3_prg_ram_enable() *(unsigned char*)0xa001 = 0b10000000

#endif // MMC3_H_
