# Format: (programmatic name, bank name, CHR-RAM address, start tile, tile count)
chr_sections = [
  ('ascii', 'data', '0x0000', 0x000, 0x80),
]

### DO NOT MODIFY BELOW ###

import sys
import lz4.frame

if len(sys.argv) != 4:
  print('Usage:', sys.argv[0], '[chr input] [header output] [asm output]')
  sys.exit(1)

with open(sys.argv[1], 'rb') as chr_in:
  header_out = open(sys.argv[2], 'w')
  asm_out = open(sys.argv[3], 'w')

  header_out.write("""
#if !defined(CHR_H_)
#define CHR_H_

#include "neslib.h"
""")

  for section in chr_sections:
    var_name = 'chr_{}_{}'.format(section[1], section[0])
    section_start = section[3] * 16
    section_size = section[4] * 16

    chr_in.seek(section_start)
    # need to skip 11 bytes: 4 MagicNb + 3 F. Descriptor + 4 Block Size
    # also skip the last 4 bytes: EndMark
    compressed_section = lz4.frame.compress(chr_in.read(section_size), compression_level=16, store_size=False)[11:-4]

    header_out.write("""
#define {var_name}_start {section_offset}
#define {var_name}_size {section_size}
extern const unsigned char {var_name}[{compressed_size}];
#define {var_name}_vram_write() \\
{{ \\
  vram_unlz4({var_name}, (unsigned char*) {var_name}_start, {var_name}_size); \\
}}
""".format(var_name=var_name, section_offset=section[2], section_size=section_size, compressed_size=len(compressed_section)))
    
    compressed_string = ','.join(['${:0>2x}'.format(c) for c in compressed_section])
    asm_out.write("""
.segment "{section_bank}"
  .export _{var_name}
_{var_name}:
  .byte {compressed_string}
""".format(section_bank=section[1].upper(), var_name=var_name, compressed_string=compressed_string))

  header_out.write("\n#endif // CHR_H_\n")

  asm_out.close()
  header_out.close()
