#include <stdbool.h>

extern void  __afailed (const char*, unsigned);







typedef unsigned size_t;

typedef struct {
    int rem;
    int quot;
} div_t;


typedef struct {
    long rem;
    long quot;
} ldiv_t;


void*  malloc (size_t size);
void*  calloc (size_t count, size_t size);
void*  realloc (void* block, size_t size);
void  free (void* block);




int  posix_memalign (void** memptr, size_t alignment, size_t size);

void  __heapadd (void* mem, size_t size);






size_t  __heapblocksize (const void* block);






size_t __heapmemavail (void);






size_t __heapmaxavail (void);

int rand (void);
void  srand (unsigned seed);
void __randomize (void);






void abort (void) __attribute__ ((noreturn));
int  abs (int val);
long  labs (long val);
int  atoi (const char* s);
long  atol (const char* s);
int  atexit (void (*exitfunc) (void));
void*  bsearch (const void* key, const void* base, size_t n,
                            size_t size, int  (* cmp) (const void*, const void*));
div_t  div (int numer, int denom);
void  exit (int ret) __attribute__ ((noreturn));
char*  getenv (const char* name);
void  qsort (void* base, size_t count, size_t size,
                         int  (* compare) (const void*, const void*));
long  strtol (const char* nptr, char** endptr, int base);
unsigned long  strtoul (const char* nptr, char** endptr, int base);
int  system (const char* s);


void  __swap (void* p, void* q, size_t size);





char*  itoa (int val, char* buf, int radix);
char*  utoa (unsigned val, char* buf, int radix);
char*  ltoa (long val, char* buf, int radix);
char*  ultoa (unsigned long val, char* buf, int radix);
int  putenv (char* s);




char*  strcat (char* dest, const char* src);
char*  strchr (const char* s, int c);
int  strcmp (const char* s1, const char* s2);
int  strcoll (const char* s1, const char* s2);
char*  strcpy (char* dest, const char* src);
size_t  strcspn (const char* s1, const char* s2);
char*  strerror (int errcode);
size_t  strlen (const char* s);
char*  strncat (char* s1, const char* s2, size_t count);
int  strncmp (const char* s1, const char* s2, size_t count);
char*  strncpy (char* dest, const char* src, size_t count);
char*  strpbrk (const char* str, const char* set);
char*  strrchr (const char* s, int c);
size_t  strspn (const char* s1, const char* s2);
char*  strstr (const char* str, const char* substr);
char*  strtok (char* s1, const char* s2);
size_t  strxfrm (char* s1, const char* s2, size_t count);
void*  memchr (const void* mem, int c, size_t count);
int  memcmp (const void* p1, const void* p2, size_t count);
void*  memcpy (void* dest, const void* src, size_t count);
void*  memmove (void* dest, const void* src, size_t count);
void*  memset (void* s, int c, size_t count);




void*  __bzero (void* ptr, size_t n);



void  bzero (void* ptr, size_t n);
char*  strdup (const char* s);
int  stricmp (const char* s1, const char* s2);
int  strcasecmp (const char* s1, const char* s2);
int  strnicmp (const char* s1, const char* s2, size_t count);
int  strncasecmp (const char* s1, const char* s2, size_t count);
size_t  strnlen (const char* s, size_t maxlen);
char*  strlwr (char* s);
char*  strlower (char* s);
char*  strupr (char* s);
char*  strupper (char* s);
char*  strqtok (char* s1, const char* s2);


const char*  __stroserror (unsigned char errcode);




// typedef unsigned char _Bool;




typedef int ptrdiff_t;



typedef char wchar_t;




typedef signed char int8_t;
typedef int int16_t;
typedef long int32_t;
typedef unsigned char uint8_t;
typedef unsigned uint16_t;
typedef unsigned long uint32_t;

typedef signed char int_least8_t;
typedef int int_least16_t;
typedef long int_least32_t;
typedef unsigned char uint_least8_t;
typedef unsigned uint_least16_t;
typedef unsigned long uint_least32_t;

typedef signed char int_fast8_t;
typedef int int_fast16_t;
typedef long int_fast32_t;
typedef unsigned char uint_fast8_t;
typedef unsigned uint_fast16_t;
typedef unsigned long uint_fast32_t;

typedef int intptr_t;
typedef unsigned uintptr_t;






typedef long intmax_t;
typedef unsigned long uintmax_t;




enum qrcodegen_Ecc {


 qrcodegen_Ecc_LOW = 0 ,
 qrcodegen_Ecc_MEDIUM ,
 qrcodegen_Ecc_QUARTILE,
 qrcodegen_Ecc_HIGH ,
};





enum qrcodegen_Mask {


 qrcodegen_Mask_AUTO = -1,

 qrcodegen_Mask_0 = 0,
 qrcodegen_Mask_1,
 qrcodegen_Mask_2,
 qrcodegen_Mask_3,
 qrcodegen_Mask_4,
 qrcodegen_Mask_5,
 qrcodegen_Mask_6,
 qrcodegen_Mask_7,
};





enum qrcodegen_Mode {
 qrcodegen_Mode_NUMERIC = 0x1,
 qrcodegen_Mode_ALPHANUMERIC = 0x2,
 qrcodegen_Mode_BYTE = 0x4,
 qrcodegen_Mode_KANJI = 0x8,
 qrcodegen_Mode_ECI = 0x7,
};

struct qrcodegen_Segment {

 enum qrcodegen_Mode mode;




 int numChars;



 uint8_t *data;




 int bitLength;
};

_Bool qrcodegen_encodeText(const char *text, uint8_t tempBuffer[], uint8_t qrcode[],
 enum qrcodegen_Ecc ecl, int minVersion, int maxVersion, enum qrcodegen_Mask mask, _Bool boostEcl);

_Bool qrcodegen_encodeBinary(uint8_t dataAndTemp[], size_t dataLen, uint8_t qrcode[],
 enum qrcodegen_Ecc ecl, int minVersion, int maxVersion, enum qrcodegen_Mask mask, _Bool boostEcl);

_Bool qrcodegen_encodeSegments(const struct qrcodegen_Segment segs[], size_t len,
 enum qrcodegen_Ecc ecl, uint8_t tempBuffer[], uint8_t qrcode[]);

_Bool qrcodegen_encodeSegmentsAdvanced(const struct qrcodegen_Segment segs[], size_t len, enum qrcodegen_Ecc ecl,
 int minVersion, int maxVersion, enum qrcodegen_Mask mask, _Bool boostEcl, uint8_t tempBuffer[], uint8_t qrcode[]);






_Bool qrcodegen_isNumeric(const char *text);

_Bool qrcodegen_isAlphanumeric(const char *text);

size_t qrcodegen_calcSegmentBufferSize(enum qrcodegen_Mode mode, size_t numChars);

struct qrcodegen_Segment qrcodegen_makeBytes(const uint8_t data[], size_t len, uint8_t buf[]);





struct qrcodegen_Segment qrcodegen_makeNumeric(const char *digits, uint8_t buf[]);

struct qrcodegen_Segment qrcodegen_makeAlphanumeric(const char *text, uint8_t buf[]);






struct qrcodegen_Segment qrcodegen_makeEci(long assignVal, uint8_t buf[]);

int qrcodegen_getSize(const uint8_t qrcode[]);

_Bool qrcodegen_getModule(const uint8_t qrcode[], int x, int y);









static void appendBitsToBuffer(unsigned int val, int numBits, uint8_t buffer[], int *bitLen);

static void addEccAndInterleave(uint8_t data[], int version, enum qrcodegen_Ecc ecl, uint8_t result[]);
static int getNumDataCodewords(int version, enum qrcodegen_Ecc ecl);
static int getNumRawDataModules(int ver);

static void reedSolomonComputeDivisor(int degree, uint8_t result[]);
static void reedSolomonComputeRemainder(const uint8_t data[], int dataLen,
 const uint8_t generator[], int degree, uint8_t result[]);
static uint8_t reedSolomonMultiply(uint8_t x, uint8_t y);

static void initializeFunctionModules(int version, uint8_t qrcode[]);
static void drawLightFunctionModules(uint8_t qrcode[], int version);
static void drawFormatBits(enum qrcodegen_Ecc ecl, enum qrcodegen_Mask mask, uint8_t qrcode[]);
static int getAlignmentPatternPositions(int version, uint8_t result[7]);
static void fillRectangle(int left, int top, int width, int height, uint8_t qrcode[]);

static void drawCodewords(const uint8_t data[], int dataLen, uint8_t qrcode[]);
static void applyMask(const uint8_t functionModules[], uint8_t qrcode[], enum qrcodegen_Mask mask);
static long getPenaltyScore(const uint8_t qrcode[]);
static int finderPenaltyCountPatterns(const int runHistory[7], int qrsize);
static int finderPenaltyTerminateAndCount(_Bool currentRunColor, int currentRunLength, int runHistory[7], int qrsize);
static void finderPenaltyAddHistory(int currentRunLength, int runHistory[7], int qrsize);

static _Bool getModuleBounded(const uint8_t qrcode[], int x, int y);
static void setModuleBounded(uint8_t qrcode[], int x, int y, _Bool isDark);
static void setModuleUnbounded(uint8_t qrcode[], int x, int y, _Bool isDark);
static _Bool getBit(int x, int i);

static int calcSegmentBitLength(enum qrcodegen_Mode mode, size_t numChars);
static int getTotalBits(const struct qrcodegen_Segment segs[], size_t len, int version);
static int numCharCountBits(enum qrcodegen_Mode mode, int version);

static const char *ALPHANUMERIC_CHARSET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";





static const int8_t ECC_CODEWORDS_PER_BLOCK[4][41] = {


 {-1, 7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
 {-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28},
 {-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
 {-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
};




static const int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41] = {


 {-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 6, 6, 6, 6, 7, 8, 8, 9, 9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},
 {-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5, 5, 8, 9, 9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},
 {-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8, 8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},
 {-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},
};


static const int PENALTY_N1 = 3;
static const int PENALTY_N2 = 3;
static const int PENALTY_N3 = 40;
static const int PENALTY_N4 = 10;

uint8_t paddedQrSize;







_Bool qrcodegen_encodeBinary(uint8_t dataAndTemp[], size_t dataLen, uint8_t qrcode[],
  enum qrcodegen_Ecc ecl, int minVersion, int maxVersion, enum qrcodegen_Mask mask, _Bool boostEcl) {

 struct qrcodegen_Segment seg;
 seg.mode = qrcodegen_Mode_BYTE;
 seg.bitLength = calcSegmentBitLength(seg.mode, dataLen);
 if (seg.bitLength == -1) {
  qrcode[0] = 0;
  return 0;
 }
 seg.numChars = (int)dataLen;
 seg.data = dataAndTemp;
 return qrcodegen_encodeSegmentsAdvanced(&seg, 1, ecl, minVersion, maxVersion, mask, boostEcl, dataAndTemp, qrcode);
}




static void appendBitsToBuffer(unsigned int val, int numBits, uint8_t buffer[], int *bitLen) {
 ((0 <= numBits && numBits <= 16 && (unsigned long)val >> numBits == 0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 189));
 { int i = numBits - 1; for ( ; i >= 0; i--, (*bitLen)++)
  buffer[*bitLen >> 3] |= ((val >> i) & 1) << (7 - (*bitLen & 7)); }
}






_Bool qrcodegen_encodeSegments(const struct qrcodegen_Segment segs[], size_t len,
  enum qrcodegen_Ecc ecl, uint8_t tempBuffer[], uint8_t qrcode[]) {
 return qrcodegen_encodeSegmentsAdvanced(segs, len, ecl,
  1, 40, qrcodegen_Mask_AUTO, 1, tempBuffer, qrcode);
}



_Bool qrcodegen_encodeSegmentsAdvanced(const struct qrcodegen_Segment segs[], size_t len, enum qrcodegen_Ecc ecl,
  int minVersion, int maxVersion, enum qrcodegen_Mask mask, _Bool boostEcl, uint8_t tempBuffer[], uint8_t qrcode[]) {
 ((segs != 0 || len == 0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 209));
 ((1 <= minVersion && minVersion <= maxVersion && maxVersion <= 40)? (void)0 : __afailed(".\\qrcodegen.orig.c", 210));
 ((0 <= (int)ecl && (int)ecl <= 3 && -1 <= (int)mask && (int)mask <= 7)? (void)0 : __afailed(".\\qrcodegen.orig.c", 211));


 { int version, dataUsedBits;
 for (version = minVersion; ; version++) {
  int dataCapacityBits = getNumDataCodewords(version, ecl) * 8;
  dataUsedBits = getTotalBits(segs, len, version);
  if (dataUsedBits != -1 && dataUsedBits <= dataCapacityBits)
   break;
  if (version >= maxVersion) {
   qrcode[0] = 0;
   return 0;
  }
 }
 ((dataUsedBits != -1)? (void)0 : __afailed(".\\qrcodegen.orig.c", 225));


 { int i = (int)qrcodegen_Ecc_MEDIUM; for (                    ; i <= (int)qrcodegen_Ecc_HIGH; i++) {
  if (boostEcl && dataUsedBits <= getNumDataCodewords(version, (enum qrcodegen_Ecc)i) * 8)
   ecl = (enum qrcodegen_Ecc)i;
 } }


 memset(qrcode, 0, (size_t)((((version) * 4 + 17) * ((version) * 4 + 17) + 7) / 8 + 1) * sizeof(qrcode[0]));
 { int bitLen = 0;
 { size_t i = 0; for ( ; i < len; i++) {
  const struct qrcodegen_Segment *seg = &segs[i];
  appendBitsToBuffer((unsigned int)seg->mode, 4, qrcode, &bitLen);
  appendBitsToBuffer((unsigned int)seg->numChars, numCharCountBits(seg->mode, version), qrcode, &bitLen);
  { int j = 0; for ( ; j < seg->bitLength; j++) {
   int bit = (seg->data[j >> 3] >> (7 - (j & 7))) & 1;
   appendBitsToBuffer((unsigned int)bit, 1, qrcode, &bitLen);
  } }
 } }
 ((bitLen == dataUsedBits)? (void)0 : __afailed(".\\qrcodegen.orig.c", 245));


 { int dataCapacityBits = getNumDataCodewords(version, ecl) * 8;
 ((bitLen <= dataCapacityBits)? (void)0 : __afailed(".\\qrcodegen.orig.c", 249));
 { int terminatorBits = dataCapacityBits - bitLen;
 if (terminatorBits > 4)
  terminatorBits = 4;
 appendBitsToBuffer(0, terminatorBits, qrcode, &bitLen);
 appendBitsToBuffer(0, (8 - bitLen % 8) % 8, qrcode, &bitLen);
 ((bitLen % 8 == 0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 255));


 { uint8_t padByte = 0xEC; for (    ; bitLen < dataCapacityBits; padByte ^= 0xEC ^ 0x11)
  appendBitsToBuffer(padByte, 8, qrcode, &bitLen); }


 addEccAndInterleave(qrcode, version, ecl, tempBuffer);
 initializeFunctionModules(version, qrcode);
 drawCodewords(tempBuffer, getNumRawDataModules(version) / 8, qrcode);
 drawLightFunctionModules(qrcode, version);
 initializeFunctionModules(version, tempBuffer);


 if (mask == qrcodegen_Mask_AUTO) {
  long minPenalty = 2147483647L;
  { int i = 0; for ( ; i < 8; i++) {
   enum qrcodegen_Mask msk = (enum qrcodegen_Mask)i;
   applyMask(tempBuffer, qrcode, msk);
   drawFormatBits(ecl, msk, qrcode);
   { long penalty = getPenaltyScore(qrcode);
   if (penalty < minPenalty) {
    mask = msk;
    minPenalty = penalty;
   }
   applyMask(tempBuffer, qrcode, msk);
  } } }
 }
 ((0 <= (int)mask && (int)mask <= 7)? (void)0 : __afailed(".\\qrcodegen.orig.c", 283));
 applyMask(tempBuffer, qrcode, mask);
 drawFormatBits(ecl, mask, qrcode);
 return 1;
} } } } }

static void addEccAndInterleave(uint8_t data[], int version, enum qrcodegen_Ecc ecl, uint8_t result[]) {

 ((0 <= (int)ecl && (int)ecl < 4 && 1 <= version && version <= 40)? (void)0 : __afailed(".\\qrcodegen.orig.c", 299));
 { int numBlocks = NUM_ERROR_CORRECTION_BLOCKS[(int)ecl][version];
 { int blockEccLen = ECC_CODEWORDS_PER_BLOCK [(int)ecl][version];
 { int rawCodewords = getNumRawDataModules(version) / 8;
 { int dataLen = getNumDataCodewords(version, ecl);
 { int numShortBlocks = numBlocks - rawCodewords % numBlocks;
 { int shortBlockDataLen = rawCodewords / numBlocks - blockEccLen;



 { uint8_t rsdiv[30];
 reedSolomonComputeDivisor(blockEccLen, rsdiv);
 { const uint8_t *dat = data;
 { int i = 0; for ( ; i < numBlocks; i++) {
  int datLen = shortBlockDataLen + (i < numShortBlocks ? 0 : 1);
  uint8_t *ecc = &data[dataLen];
  reedSolomonComputeRemainder(dat, datLen, rsdiv, blockEccLen, ecc);
  { int j = 0, k = i; for ( ; j < datLen; j++, k += numBlocks) {
   if (j == shortBlockDataLen)
    k -= numShortBlocks;
   result[k] = dat[j];
  } }
  { int j = 0, k = dataLen + i; for ( ; j < blockEccLen; j++, k += numBlocks)
   result[k] = ecc[j]; }
  dat += datLen;
 } }
} } } } } } } } }




static int getNumDataCodewords(int version, enum qrcodegen_Ecc ecl) {
 int v = version, e = (int)ecl;
 ((0 <= e && e < 4)? (void)0 : __afailed(".\\qrcodegen.orig.c", 332));
 return getNumRawDataModules(v) / 8
  - ECC_CODEWORDS_PER_BLOCK [e][v]
  * NUM_ERROR_CORRECTION_BLOCKS[e][v];
}





static int getNumRawDataModules(int ver) {
 ((1 <= ver && ver <= 40)? (void)0 : __afailed(".\\qrcodegen.orig.c", 343));
 { int result = (16 * ver + 128) * ver + 64;
 if (ver >= 2) {
  int numAlign = ver / 7 + 2;
  result -= (25 * numAlign - 10) * numAlign - 55;
  if (ver >= 7)
   result -= 36;
 }
 ((208 <= result && result <= 29648)? (void)0 : __afailed(".\\qrcodegen.orig.c", 351));
 return result;
} }

static void reedSolomonComputeDivisor(int degree, uint8_t result[]) {
 ((1 <= degree && degree <= 30)? (void)0 : __afailed(".\\qrcodegen.orig.c", 362));


 memset(result, 0, (size_t)degree * sizeof(result[0]));
 result[degree - 1] = 1;




 { uint8_t root = 1;
 { int i = 0; for ( ; i < degree; i++) {

  { int j = 0; for ( ; j < degree; j++) {
   result[j] = reedSolomonMultiply(result[j], root);
   if (j + 1 < degree)
    result[j] ^= result[j + 1];
  } }
  root = reedSolomonMultiply(root, 0x02);
 } }
} }





static void reedSolomonComputeRemainder(const uint8_t data[], int dataLen,
  const uint8_t generator[], int degree, uint8_t result[]) {
 ((1 <= degree && degree <= 30)? (void)0 : __afailed(".\\qrcodegen.orig.c", 389));
 memset(result, 0, (size_t)degree * sizeof(result[0]));
 { int i = 0; for ( ; i < dataLen; i++) {
  uint8_t factor = data[i] ^ result[0];
  memmove(&result[0], &result[1], (size_t)(degree - 1) * sizeof(result[0]));
  result[degree - 1] = 0;
  { int j = 0; for ( ; j < degree; j++)
   result[j] ^= reedSolomonMultiply(generator[j], factor); }
 } }
}






static uint8_t reedSolomonMultiply(uint8_t x, uint8_t y) {

 uint8_t z = 0;
 { int i = 7; for ( ; i >= 0; i--) {
  z = (uint8_t)((z << 1) ^ ((z >> 7) * 0x11D));
  z ^= ((y >> i) & 1) * x;
 } }
 return z;
}

static void initializeFunctionModules(int version, uint8_t qrcode[]) {

 int qrsize = version * 4 + 17;
 paddedQrSize = qrsize;
 if (paddedQrSize & 0x7 != 0)
 {
  paddedQrSize = (paddedQrSize & ~7) + 8;
 }
 
 memset(qrcode, 0, (size_t)((qrsize * qrsize + 7) / 8 + 1) * sizeof(qrcode[0]));
 qrcode[0] = (uint8_t)qrsize;


 fillRectangle(6, 0, 1, qrsize, qrcode);
 fillRectangle(0, 6, qrsize, 1, qrcode);


 fillRectangle(0, 0, 9, 9, qrcode);
 fillRectangle(qrsize - 8, 0, 8, 9, qrcode);
 fillRectangle(0, qrsize - 8, 9, 8, qrcode);


 { uint8_t alignPatPos[7];
 { int numAlign = getAlignmentPatternPositions(version, alignPatPos);
 { int i = 0; for ( ; i < numAlign; i++) {
  { int j = 0; for ( ; j < numAlign; j++) {

   if (!((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0)))
    fillRectangle(alignPatPos[i] - 2, alignPatPos[j] - 2, 5, 5, qrcode);
  } }
 } }


 if (version >= 7) {
  fillRectangle(qrsize - 11, 0, 3, 6, qrcode);
  fillRectangle(0, qrsize - 11, 6, 3, qrcode);
 }
} } }





static void drawLightFunctionModules(uint8_t qrcode[], int version) {

 int qrsize = qrcodegen_getSize(qrcode);
 { int i = 7; for ( ; i < qrsize - 7; i += 2) {
  setModuleBounded(qrcode, 6, i, 0);
  setModuleBounded(qrcode, i, 6, 0);
 } }


 { int dy = -4; for ( ; dy <= 4; dy++) {
  { int dx = -4; for ( ; dx <= 4; dx++) {
   int dist = abs(dx);
   if (abs(dy) > dist)
    dist = abs(dy);
   if (dist == 2 || dist == 4) {
    setModuleUnbounded(qrcode, 3 + dx, 3 + dy, 0);
    setModuleUnbounded(qrcode, qrsize - 4 + dx, 3 + dy, 0);
    setModuleUnbounded(qrcode, 3 + dx, qrsize - 4 + dy, 0);
   }
  } }
 } }


 { uint8_t alignPatPos[7];
 { int numAlign = getAlignmentPatternPositions(version, alignPatPos);
 { int i = 0; for ( ; i < numAlign; i++) {
  { int j = 0; for ( ; j < numAlign; j++) {
   if ((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0))
    continue;
   { int dy = -1; for ( ; dy <= 1; dy++) {
    { int dx = -1; for ( ; dx <= 1; dx++)
     setModuleBounded(qrcode, alignPatPos[i] + dx, alignPatPos[j] + dy, dx == 0 && dy == 0); }
   } }
  } }
 } }


 if (version >= 7) {

  int rem = version;
  { int i = 0; for ( ; i < 12; i++)
   rem = (rem << 1) ^ ((rem >> 11) * 0x1F25); }
  { long bits = (long)version << 12 | rem;
  ((bits >> 18 == 0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 501));


  { int i = 0; for ( ; i < 6; i++) {
   { int j = 0; for ( ; j < 3; j++) {
    int k = qrsize - 11 + j;
    setModuleBounded(qrcode, k, i, (bits & 1) != 0);
    setModuleBounded(qrcode, i, k, (bits & 1) != 0);
    bits >>= 1;
   } }
  } }
 } }
} } }





static void drawFormatBits(enum qrcodegen_Ecc ecl, enum qrcodegen_Mask mask, uint8_t qrcode[]) {

 ((0 <= (int)mask && (int)mask <= 7)? (void)0 : __afailed(".\\qrcodegen.orig.c", 521));
 { static const int table[] = {1, 0, 3, 2};
 { int data = table[(int)ecl] << 3 | (int)mask;
 { int rem = data;
 { int i = 0; for ( ; i < 10; i++)
  rem = (rem << 1) ^ ((rem >> 9) * 0x537); }
 { int bits = (data << 10 | rem) ^ 0x5412;
 ((bits >> 15 == 0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 528));


 { int i = 0; for ( ; i <= 5; i++)
  setModuleBounded(qrcode, 8, i, getBit(bits, i)); }
 setModuleBounded(qrcode, 8, 7, getBit(bits, 6));
 setModuleBounded(qrcode, 8, 8, getBit(bits, 7));
 setModuleBounded(qrcode, 7, 8, getBit(bits, 8));
 { int i = 9; for ( ; i < 15; i++)
  setModuleBounded(qrcode, 14 - i, 8, getBit(bits, i)); }


 { int qrsize = qrcodegen_getSize(qrcode);
 { int i = 0; for ( ; i < 8; i++)
  setModuleBounded(qrcode, qrsize - 1 - i, 8, getBit(bits, i)); }
 { int i = 8; for ( ; i < 15; i++)
  setModuleBounded(qrcode, 8, qrsize - 15 + i, getBit(bits, i)); }
 setModuleBounded(qrcode, 8, qrsize - 8, 1);
} } } } } }






static int getAlignmentPatternPositions(int version, uint8_t result[7]) {
 if (version == 1)
  return 0;
 { int numAlign = version / 7 + 2;
 { int step = (version == 32) ? 26 :
  (version * 4 + numAlign * 2 + 1) / (numAlign * 2 - 2) * 2;
 { int i = numAlign - 1, pos = version * 4 + 10; for (  ; i >= 1; i--, pos -= step)
  result[i] = (uint8_t)pos; }
 result[0] = 6;
 return numAlign;
} } }



static void fillRectangle(int left, int top, int width, int height, uint8_t qrcode[]) {
 { int dy = 0; for ( ; dy < height; dy++) {
  { int dx = 0; for ( ; dx < width; dx++)
   setModuleBounded(qrcode, left + dx, top + dy, 1); }
 } }
}

static void drawCodewords(const uint8_t data[], int dataLen, uint8_t qrcode[]) {
 int qrsize = qrcodegen_getSize(qrcode);
 int i = 0;

 { int right = qrsize - 1; for ( ; right >= 1; right -= 2) {
  if (right == 6)
   right = 5;
  { int vert = 0; for ( ; vert < qrsize; vert++) {
   { int j = 0; for ( ; j < 2; j++) {
    int x = right - j;
    _Bool upward = ((right + 1) & 2) == 0;
    int y = upward ? qrsize - 1 - vert : vert;
    if (!getModuleBounded(qrcode, x, y) && i < dataLen * 8) {
     _Bool dark = getBit(data[i >> 3], 7 - (i & 7));
     setModuleBounded(qrcode, x, y, dark);
     i++;
    }


   } }
  } }
 } }
 ((i == dataLen * 8)? (void)0 : __afailed(".\\qrcodegen.orig.c", 602));
}

static void applyMask(const uint8_t functionModules[], uint8_t qrcode[], enum qrcodegen_Mask mask) {
 ((0 <= (int)mask && (int)mask <= 7)? (void)0 : __afailed(".\\qrcodegen.orig.c", 612));
 { int qrsize = qrcodegen_getSize(qrcode);
 { int y = 0; for ( ; y < qrsize; y++) {
  { int x = 0; for ( ; x < qrsize; x++) {
   if (getModuleBounded(functionModules, x, y))
    continue;
   { _Bool invert;
   switch ((int)mask) {
    case 0: invert = (x + y) % 2 == 0; break;
    case 1: invert = y % 2 == 0; break;
    case 2: invert = x % 3 == 0; break;
    case 3: invert = (x + y) % 3 == 0; break;
    case 4: invert = (x / 3 + y / 2) % 2 == 0; break;
    case 5: invert = x * y % 2 + x * y % 3 == 0; break;
    case 6: invert = (x * y % 2 + x * y % 3) % 2 == 0; break;
    case 7: invert = ((x + y) % 2 + x * y % 3) % 2 == 0; break;
    default: ((0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 628)); return;
   }
   { _Bool val = getModuleBounded(qrcode, x, y);
   setModuleBounded(qrcode, x, y, val ^ invert);
  } } } }
 } }
} }




static long getPenaltyScore(const uint8_t qrcode[]) {
 int qrsize = qrcodegen_getSize(qrcode);
 long result = 0;


 { int y = 0; for ( ; y < qrsize; y++) {
  _Bool runColor = 0;
  int runX = 0;
  int runHistory[7] = {0};
  { int x = 0; for ( ; x < qrsize; x++) {
   if (getModuleBounded(qrcode, x, y) == runColor) {
    runX++;
    if (runX == 5)
     result += PENALTY_N1;
    else if (runX > 5)
     result++;
   } else {
    finderPenaltyAddHistory(runX, runHistory, qrsize);
    if (!runColor)
     result += finderPenaltyCountPatterns(runHistory, qrsize) * PENALTY_N3;
    runColor = getModuleBounded(qrcode, x, y);
    runX = 1;
   }
  } }
  result += finderPenaltyTerminateAndCount(runColor, runX, runHistory, qrsize) * PENALTY_N3;
 } }

 { int x = 0; for ( ; x < qrsize; x++) {
  _Bool runColor = 0;
  int runY = 0;
  int runHistory[7] = {0};
  { int y = 0; for ( ; y < qrsize; y++) {
   if (getModuleBounded(qrcode, x, y) == runColor) {
    runY++;
    if (runY == 5)
     result += PENALTY_N1;
    else if (runY > 5)
     result++;
   } else {
    finderPenaltyAddHistory(runY, runHistory, qrsize);
    if (!runColor)
     result += finderPenaltyCountPatterns(runHistory, qrsize) * PENALTY_N3;
    runColor = getModuleBounded(qrcode, x, y);
    runY = 1;
   }
  } }
  result += finderPenaltyTerminateAndCount(runColor, runY, runHistory, qrsize) * PENALTY_N3;
 } }


 { int y = 0; for ( ; y < qrsize - 1; y++) {
  { int x = 0; for ( ; x < qrsize - 1; x++) {
   _Bool color = getModuleBounded(qrcode, x, y);
   if ( color == getModuleBounded(qrcode, x + 1, y) &&
         color == getModuleBounded(qrcode, x, y + 1) &&
         color == getModuleBounded(qrcode, x + 1, y + 1))
    result += PENALTY_N2;
  } }
 } }


 { int dark = 0;
 { int y = 0; for ( ; y < qrsize; y++) {
  { int x = 0; for ( ; x < qrsize; x++) {
   if (getModuleBounded(qrcode, x, y))
    dark++;
  } }
 } }
 { int total = qrsize * qrsize;

 { int k = (int)((labs(dark * 20L - total * 10L) + total - 1) / total) - 1;
 ((0 <= k && k <= 9)? (void)0 : __afailed(".\\qrcodegen.orig.c", 710));
 result += k * PENALTY_N4;
 ((0 <= result && result <= 2568888L)? (void)0 : __afailed(".\\qrcodegen.orig.c", 712));
 return result;
} } } }




static int finderPenaltyCountPatterns(const int runHistory[7], int qrsize) {
 int n = runHistory[1];
 ((n <= qrsize * 3)? (void)0 : __afailed(".\\qrcodegen.orig.c", 721)); (void)qrsize;
 { _Bool core = n > 0 && runHistory[2] == n && runHistory[3] == n * 3 && runHistory[4] == n && runHistory[5] == n;


 return (core && runHistory[0] >= n * 4 && runHistory[6] >= n ? 1 : 0)
      + (core && runHistory[6] >= n * 4 && runHistory[0] >= n ? 1 : 0);
} }



static int finderPenaltyTerminateAndCount(_Bool currentRunColor, int currentRunLength, int runHistory[7], int qrsize) {
 if (currentRunColor) {
  finderPenaltyAddHistory(currentRunLength, runHistory, qrsize);
  currentRunLength = 0;
 }
 currentRunLength += qrsize;
 finderPenaltyAddHistory(currentRunLength, runHistory, qrsize);
 return finderPenaltyCountPatterns(runHistory, qrsize);
}



static void finderPenaltyAddHistory(int currentRunLength, int runHistory[7], int qrsize) {
 if (runHistory[0] == 0)
  currentRunLength += qrsize;
 memmove(&runHistory[1], &runHistory[0], 6 * sizeof(runHistory[0]));
 runHistory[0] = currentRunLength;
}






int qrcodegen_getSize(const uint8_t qrcode[]) {
 ((qrcode != 0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 756));
 { int result = qrcode[0];
 (((1 * 4 + 17) <= result && result <= (40 * 4 + 17))? (void)0 : __afailed(".\\qrcodegen.orig.c", 759))
                                                ;
 return result;
} }



_Bool qrcodegen_getModule(const uint8_t qrcode[], int x, int y) {
 ((qrcode != 0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 766));
 { int qrsize = qrcode[0];
 return (0 <= x && x < qrsize && 0 <= y && y < qrsize) && getModuleBounded(qrcode, x, y);
} }



static _Bool getModuleBounded(const uint8_t qrcode[], int x, int y) {
 int qrsize = qrcode[0];
 ((21 <= qrsize && qrsize <= 177 && 0 <= x && x < qrsize && 0 <= y && y < qrsize)? (void)0 : __afailed(".\\qrcodegen.orig.c", 775));
 { int index = y * paddedQrSize + x;
 return getBit(qrcode[(index >> 3) + 1], index & 7);
} }



static void setModuleBounded(uint8_t qrcode[], int x, int y, _Bool isDark) {
 int qrsize = qrcode[0];
 ((21 <= qrsize && qrsize <= 177 && 0 <= x && x < qrsize && 0 <= y && y < qrsize)? (void)0 : __afailed(".\\qrcodegen.orig.c", 784));
 { int index = y * paddedQrSize + x;
 { int bitIndex = index & 7;
 { int byteIndex = (index >> 3) + 1;
 if (isDark)
  qrcode[byteIndex] |= 1 << bitIndex;
 else
  qrcode[byteIndex] &= (1 << bitIndex) ^ 0xFF;
} } } }



static void setModuleUnbounded(uint8_t qrcode[], int x, int y, _Bool isDark) {
 int qrsize = qrcode[0];
 if (0 <= x && x < qrsize && 0 <= y && y < qrsize)
  setModuleBounded(qrcode, x, y, isDark);
}



static _Bool getBit(int x, int i) {
 return ((x >> i) & 1) != 0;
}






_Bool qrcodegen_isNumeric(const char *text) {
 ((text != 0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 814));
 for (; *text != '\0'; text++) {
  if (*text < '0' || *text > '9')
   return 0;
 }
 return 1;
}



_Bool qrcodegen_isAlphanumeric(const char *text) {
 ((text != 0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 825));
 for (; *text != '\0'; text++) {
  if (strchr(ALPHANUMERIC_CHARSET, *text) == 0)
   return 0;
 }
 return 1;
}



size_t qrcodegen_calcSegmentBufferSize(enum qrcodegen_Mode mode, size_t numChars) {
 int temp = calcSegmentBitLength(mode, numChars);
 if (temp == -1)
  return 0xFFFF;
 ((0 <= temp && temp <= ((int16_t) 0x7FFF))? (void)0 : __afailed(".\\qrcodegen.orig.c", 839));
 return ((size_t)temp + 7) / 8;
}

static int calcSegmentBitLength(enum qrcodegen_Mode mode, size_t numChars) {

 if (numChars > (unsigned int)((int16_t) 0x7FFF))
  return -1;
 { long result = (long)numChars;
 if (mode == qrcodegen_Mode_NUMERIC)
  result = (result * 10 + 2) / 3;
 else if (mode == qrcodegen_Mode_ALPHANUMERIC)
  result = (result * 11 + 1) / 2;
 else if (mode == qrcodegen_Mode_BYTE)
  result *= 8;
 else if (mode == qrcodegen_Mode_KANJI)
  result *= 13;
 else if (mode == qrcodegen_Mode_ECI && numChars == 0)
  result = 3 * 8;
 else {
  ((0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 868));
  return -1;
 }
 ((result >= 0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 871));
 if (result > ((int16_t) 0x7FFF))
  return -1;
 return (int)result;
} }








static int getTotalBits(const struct qrcodegen_Segment segs[], size_t len, int version) {
 ((segs != 0 || len == 0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 992));
 { long result = 0;
 { size_t i = 0; for ( ; i < len; i++) {
  int numChars = segs[i].numChars;
  int bitLength = segs[i].bitLength;
  ((0 <= numChars && numChars <= ((int16_t) 0x7FFF))? (void)0 : __afailed(".\\qrcodegen.orig.c", 997));
  ((0 <= bitLength && bitLength <= ((int16_t) 0x7FFF))? (void)0 : __afailed(".\\qrcodegen.orig.c", 998));
  { int ccbits = numCharCountBits(segs[i].mode, version);
  ((0 <= ccbits && ccbits <= 16)? (void)0 : __afailed(".\\qrcodegen.orig.c", 1000));
  if (numChars >= (1L << ccbits))
   return -1;
  result += 4L + ccbits + bitLength;
  if (result > ((int16_t) 0x7FFF))
   return -1;
 } } }
 ((0 <= result && result <= ((int16_t) 0x7FFF))? (void)0 : __afailed(".\\qrcodegen.orig.c", 1007));
 return (int)result;
} }




static int numCharCountBits(enum qrcodegen_Mode mode, int version) {
 ((1 <= version && version <= 40)? (void)0 : __afailed(".\\qrcodegen.orig.c", 1015));
 { int i = (version + 7) / 17;
 switch (mode) {
  case qrcodegen_Mode_NUMERIC : { static const int temp[] = {10, 12, 14}; return temp[i]; }
  case qrcodegen_Mode_ALPHANUMERIC: { static const int temp[] = { 9, 11, 13}; return temp[i]; }
  case qrcodegen_Mode_BYTE : { static const int temp[] = { 8, 16, 16}; return temp[i]; }
  case qrcodegen_Mode_KANJI : { static const int temp[] = { 8, 10, 12}; return temp[i]; }
  case qrcodegen_Mode_ECI : return 0;
  default: ((0)? (void)0 : __afailed(".\\qrcodegen.orig.c", 1023)); return -1;
 } }
}
