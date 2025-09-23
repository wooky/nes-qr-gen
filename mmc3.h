#if !defined(MMC3_H_)
#define MMC3_H_

#define mmc3_prg_bank(section_8kb, bank_number) { \
  *(unsigned char*)0x8000 = 0b00000110 | section_8kb; \
  *(unsigned char*)0x8001 = bank_number; \
}

#define mmc3_prg_ram_enable() *(unsigned char*)0xa001 = 0b10000000

#define mmc3_chr_bank(section_2kb, bank_number) { \
  *(unsigned char*)0x8000 = 0b00000000 | section_2kb; \
  *(unsigned char*)0x8001 = bank_number; \
}

#endif // MMC3_H_
