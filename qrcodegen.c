/* 
 * QR Code generator library (C)
 * 
 * Copyright (c) Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/qr-code-generator-library
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */
/*
 * Modified for nes-qr-demo
 */

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "qrcodegen.h"

#ifndef QRCODEGEN_TEST
	#define testable static  // Keep functions private
#else
	#define testable  // Expose private functions
#endif


/*---- Forward declarations for private functions ----*/

// Regarding all public and private functions defined in this source file:
// - They require all pointer/array arguments to be not null unless the array length is zero.
// - They only read input scalar/array arguments, write to output pointer/array
//   arguments, and return scalar values; they are "pure" functions.
// - They don't read mutable global variables or write to any global variables.
// - They don't perform I/O, read the clock, print to console, etc.
// - They allocate a small and constant amount of stack memory.
// - They don't allocate or free any memory on the heap.
// - They don't recurse or mutually recurse. All the code
//   could be inlined into the top-level public functions.
// - They run in at most quadratic time with respect to input arguments.
//   Most functions run in linear time, and some in constant time.
//   There are no unbounded loops or non-obvious termination conditions.
// - They are completely thread-safe if the caller does not give the
//   same writable buffer to concurrent calls to these functions.

testable void fastcall appendBitsToQrcode(uint16_t val, uint8_t numBits);

testable void addEccAndInterleave();
testable int getNumDataCodewords(enum qrcodegen_Ecc ecl);
testable int getNumRawDataModules();

testable void reedSolomonComputeDivisor(int degree, uint8_t result[]);
testable void reedSolomonComputeRemainder();
extern uint8_t __fastcall__ reedSolomonMultiply(uint8_t x, uint8_t y);

testable void initializeFunctionModules(uint8_t buf[]);
static void drawLightFunctionModules();
static void drawFormatBits(enum qrcodegen_Mask mask);
testable int getAlignmentPatternPositions();
static void fillRectangle(int left, int top, int width, int height, uint8_t buf[]);

static void drawCodewords();
static void applyMask(enum qrcodegen_Mask mask);
static uint16_t getPenaltyScore();
static uint8_t finderPenaltyCountPatterns(const int runHistory[7]);
static int finderPenaltyTerminateAndCount(bool currentRunColor, int currentRunLength, int runHistory[7], int qrsize);
static void finderPenaltyAddHistory(int currentRunLength, int runHistory[7], int qrsize);

testable bool getModuleBounded(const uint8_t buf[], int8_t x, int8_t y);
testable void setModuleBounded(uint8_t buf[], int8_t x, int8_t y, bool isDark);
testable void setModuleUnbounded(int8_t x, int8_t y, bool isDark);
static bool fastcall getBit(int x, uint8_t i);

testable uint16_t getTotalBits();
static uint8_t numCharCountBits();



/*---- Private tables of constants ----*/

#pragma rodata-name("BANK4")

// Sentinel value for use in only some functions.
#define LENGTH_OVERFLOW -1

// For generating error correction codes.
testable const int8_t ECC_CODEWORDS_PER_BLOCK[4][41] = {
	// Version: (note that index 0 is for padding, and is set to an illegal value)
	//0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
	{-1,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Low
	{-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28},  // Medium
	{-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // Quartile
	{-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  // High
};

#define qrcodegen_REED_SOLOMON_DEGREE_MAX 30  // Based on the table above

// For generating error correction codes.
testable const int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41] = {
	// Version: (note that index 0 is for padding, and is set to an illegal value)
	//0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
	{-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},  // Low
	{-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},  // Medium
	{-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},  // Quartile
	{-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},  // High
};

// For automatic mask pattern selection.
static const int PENALTY_N1 =  3;
static const int PENALTY_N2 =  3;
static const int PENALTY_N3 = 40;
static const int PENALTY_N4 = 10;

/*---- NES-QR-DEMO: global variables  ----*/

#pragma bss-name (push, "WRAM")

#define MIN_VERSION qrcodegen_VERSION_MIN
#define MAX_VERSION 27
#define BUFFER_HEIGHT (MAX_VERSION * 4 + 17)
#define BUFFER_WIDTH 256
#define BUFFER_SIZE ((BUFFER_WIDTH * BUFFER_HEIGHT) / 8 + 1)
uint8_t tempBuffer[BUFFER_SIZE];
uint8_t qrcode[BUFFER_SIZE];

#pragma bss-name (pop)
#pragma code-name ("BANK4")

size_t dataLen;
enum qrcodegen_Ecc ecl;
enum qrcodegen_Mask mask;
bool boostEcl;

static size_t bitLength;
static int bitLen;
static uint8_t version;
static uint8_t alignPatPos[7];

extern uint8_t fastcall qr_reed_solomon_multiply(uint16_t adr);

static struct {
	union {
		struct {
			int16_t i;
			uint8_t *dest;
			uint8_t byte;
			uint8_t truncatedBitlen;
			uint8_t shift;
		} appendBitsToQrcode;
		struct {
			uint16_t index;
		} getModuleBounded;
		struct {
			uint16_t index;
			uint8_t bitIndex;
			uint16_t byteIndex;
		} setModuleBounded;
		struct {
			uint8_t *dat;
			int datLen;
			uint8_t rsdiv[qrcodegen_REED_SOLOMON_DEGREE_MAX];
			uint8_t blockEccLen;
			uint8_t *ecc;
			int shortBlockDataLen;
		} addEccAndInterleave;
	};
	union
	{
		struct
		{
			int i;
			uint8_t j;
			uint8_t factor;
		} reedSolomonComputeRemainder;
	};
	struct {
		uint16_t datLen;
		uint8_t qrsize;
		uint16_t i;
		uint8_t right;
		uint8_t vert;
		uint8_t j;
		uint8_t x;
		bool upward;
		uint8_t y;
		bool dark;
	} drawCodewords;
} d;

/*---- High-level QR Code encoding functions ----*/

// Public function - see documentation comment in header file.
bool qrcodegen_encodeBinary() {
	bitLength = dataLen * 8;
	return qrcodegen_encodeSegmentsAdvanced();
}


// Appends the given number of low-order bits of the given value to the given byte-based
// bit buffer, increasing the bit length. Requires 0 <= numBits <= 16 and val < 2^numBits.
testable void fastcall appendBitsToQrcode(uint16_t val, uint8_t numBits) {
	for (d.appendBitsToQrcode.i = numBits - 1; d.appendBitsToQrcode.i >= 0; --d.appendBitsToQrcode.i, ++bitLen) {
		d.appendBitsToQrcode.dest = &qrcode[bitLen >> 3];
		d.appendBitsToQrcode.byte = (val >> d.appendBitsToQrcode.i) & 1;
		d.appendBitsToQrcode.truncatedBitlen = bitLen & 7;
		d.appendBitsToQrcode.shift = 7 - d.appendBitsToQrcode.truncatedBitlen;
		*d.appendBitsToQrcode.dest |= d.appendBitsToQrcode.byte << d.appendBitsToQrcode.shift;
	}
}



/*---- Low-level QR Code encoding functions ----*/

// Public function - see documentation comment in header file.
bool qrcodegen_encodeSegmentsAdvanced() {
	// Find the minimal version number to use
	uint16_t dataUsedBits;
	int i, dataCapacityBits, terminatorBits;
	uint8_t padByte;
	for (version = MIN_VERSION; ; version++) {
		uint16_t dataCapacityBits = getNumDataCodewords(ecl) * 8;  // Number of data bits available
		dataUsedBits = getTotalBits();
		if (dataUsedBits != LENGTH_OVERFLOW && dataUsedBits <= dataCapacityBits)
			break;  // This version number is found to be suitable
		if (version >= MAX_VERSION) {  // All versions in the range could not fit the given data
			qrcode[0] = 0;  // Set size to invalid value for safety
			return false;
		}
	}
	
	// Increase the error correction level while the data still fits in the current version number
	for (i = (int)qrcodegen_Ecc_MEDIUM; i <= (int)qrcodegen_Ecc_HIGH; i++) {  // From low to high
		if (boostEcl && dataUsedBits <= getNumDataCodewords((enum qrcodegen_Ecc)i) * 8)
			ecl = (enum qrcodegen_Ecc)i;
	}
	
	// Concatenate all segments to create the data bit string
	memset(qrcode, 0, BUFFER_SIZE * sizeof(qrcode[0]));
	bitLen = 0;
	{
		int j;
		appendBitsToQrcode((unsigned int)qrcodegen_Mode_BYTE, 4);
		appendBitsToQrcode((unsigned int)dataLen, numCharCountBits());
		for (j = 0; j < bitLength; j++) {
			int bit = (tempBuffer[j >> 3] >> (7 - (j & 7))) & 1;
			appendBitsToQrcode((unsigned int)bit, 1);
		}
	}
	
	// Add terminator and pad up to a byte if applicable
	dataCapacityBits = getNumDataCodewords(ecl) * 8;
	terminatorBits = dataCapacityBits - bitLen;
	if (terminatorBits > 4)
		terminatorBits = 4;
	appendBitsToQrcode(0, terminatorBits);
	appendBitsToQrcode(0, (8 - bitLen % 8) % 8);
	
	// Pad with alternating bytes until data capacity is reached
	for (padByte = 0xEC; bitLen < dataCapacityBits; padByte ^= 0xEC ^ 0x11)
		appendBitsToQrcode(padByte, 8);
	
	// Compute ECC, draw modules
	addEccAndInterleave();
	initializeFunctionModules(qrcode);
	d.drawCodewords.datLen = getNumRawDataModules() / 8;
	drawCodewords();
	drawLightFunctionModules();
	initializeFunctionModules(tempBuffer);
	
	// Do masking
	if (mask == qrcodegen_Mask_AUTO) {  // Automatically choose best mask
		uint16_t minPenalty = INT16_MAX;
		for (i = 0; i < 8; i++) {
			uint16_t penalty;
			enum qrcodegen_Mask msk = (enum qrcodegen_Mask)i;
			applyMask(msk);
			drawFormatBits(msk);
			penalty = getPenaltyScore();
			if (penalty < minPenalty) {
				mask = msk;
				minPenalty = penalty;
			}
			applyMask(msk);  // Undoes the mask due to XOR
		}
	}
	applyMask(mask);  // Apply the final choice of mask
	drawFormatBits(mask);  // Overwrite old format bits
	return true;
}



/*---- Error correction code generation functions ----*/

// Appends error correction bytes to each block of the given data array, then interleaves
// bytes from the blocks and stores them in the result array. data[0 : dataLen] contains
// the input data. data[dataLen : rawCodewords] is used as a temporary work area and will
// be clobbered by this function. The final answer is stored in result[0 : rawCodewords].
testable void addEccAndInterleave() {
	// Calculate parameter numbers
	uint8_t numBlocks = NUM_ERROR_CORRECTION_BLOCKS[ecl][version];
	int rawCodewords = getNumRawDataModules() / 8;
	int dataLen = getNumDataCodewords(ecl);
	int numShortBlocks = numBlocks - rawCodewords % numBlocks;
	int i;
	d.addEccAndInterleave.blockEccLen = ECC_CODEWORDS_PER_BLOCK  [ecl][version];
	d.addEccAndInterleave.shortBlockDataLen = rawCodewords / numBlocks - d.addEccAndInterleave.blockEccLen;
	
	// Split data into blocks, calculate ECC, and interleave
	// (not concatenate) the bytes into a single sequence
	reedSolomonComputeDivisor(d.addEccAndInterleave.blockEccLen, d.addEccAndInterleave.rsdiv);
	d.addEccAndInterleave.dat = qrcode;
	for (i = 0; i < numBlocks; i++) {
		int j, k;
		d.addEccAndInterleave.datLen = d.addEccAndInterleave.shortBlockDataLen + (i < numShortBlocks ? 0 : 1);
		d.addEccAndInterleave.ecc = &qrcode[dataLen];  // Temporary storage
		reedSolomonComputeRemainder();
		for (j = 0, k = i; j < d.addEccAndInterleave.datLen; j++, k += numBlocks) {  // Copy data
			if (j == d.addEccAndInterleave.shortBlockDataLen)
				k -= numShortBlocks;
			tempBuffer[k] = d.addEccAndInterleave.dat[j];
		}
		for (j = 0, k = dataLen + i; j < d.addEccAndInterleave.blockEccLen; j++, k += numBlocks)  // Copy ECC
			tempBuffer[k] = d.addEccAndInterleave.ecc[j];
		d.addEccAndInterleave.dat += d.addEccAndInterleave.datLen;
	}
}


// Returns the number of 8-bit codewords that can be used for storing data (not ECC),
// for the given version number and error correction level. The result is in the range [9, 2956].
testable int getNumDataCodewords(enum qrcodegen_Ecc ecl) {
	return getNumRawDataModules() / 8
		- ECC_CODEWORDS_PER_BLOCK    [ecl][version]
		* NUM_ERROR_CORRECTION_BLOCKS[ecl][version];
}


// Returns the number of data bits that can be stored in a QR Code of the given version number, after
// all function modules are excluded. This includes remainder bits, so it might not be a multiple of 8.
// The result is in the range [208, 29648]. This could be implemented as a 40-entry lookup table.
testable int getNumRawDataModules() {
	int result = (16 * version + 128) * version + 64;
	if (version >= 2) {
		int numAlign = version / 7 + 2;
		result -= (25 * numAlign - 10) * numAlign - 55;
		if (version >= 7)
			result -= 36;
	}
	return result;
}



/*---- Reed-Solomon ECC generator functions ----*/

// Computes a Reed-Solomon ECC generator polynomial for the given degree, storing in result[0 : degree].
// This could be implemented as a lookup table over all possible parameter values, instead of as an algorithm.
testable void reedSolomonComputeDivisor(int degree, uint8_t result[]) {
	uint8_t root;
	int i, j;
	// Polynomial coefficients are stored from highest to lowest power, excluding the leading term which is always 1.
	// For example the polynomial x^3 + 255x^2 + 8x + 93 is stored as the uint8 array {255, 8, 93}.
	memset(result, 0, (size_t)degree * sizeof(result[0]));
	result[degree - 1] = 1;  // Start off with the monomial x^0
	
	// Compute the product polynomial (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}),
	// drop the highest monomial term which is always 1x^degree.
	// Note that r = 0x02, which is a generator element of this field GF(2^8/0x11D).
	root = 1;
	for (i = 0; i < degree; i++) {
		// Multiply the current product by (x - r^i)
		for (j = 0; j < degree; j++) {
			result[j] = reedSolomonMultiply(result[j], root);
			if (j + 1 < degree)
				result[j] ^= result[j + 1];
		}
		root = reedSolomonMultiply(root, 0x02);
	}
}


// Computes the Reed-Solomon error correction codeword for the given data and divisor polynomials.
// The remainder when data[0 : dataLen] is divided by divisor[0 : degree] is stored in result[0 : degree].
// All polynomials are in big endian, and the generator has an implicit leading 1 term.
testable void reedSolomonComputeRemainder(/*const uint8_t data[], int dataLen,
		const uint8_t generator[], int degree, uint8_t result[]*/) {
	memset(d.addEccAndInterleave.ecc, 0, (size_t)d.addEccAndInterleave.blockEccLen * sizeof(d.addEccAndInterleave.ecc[0]));
	for (d.reedSolomonComputeRemainder.i = 0; d.reedSolomonComputeRemainder.i < d.addEccAndInterleave.datLen; ++d.reedSolomonComputeRemainder.i) {  // Polynomial division
		d.reedSolomonComputeRemainder.factor = d.addEccAndInterleave.dat[d.reedSolomonComputeRemainder.i] ^ d.addEccAndInterleave.ecc[0];
		memmove(&d.addEccAndInterleave.ecc[0], &d.addEccAndInterleave.ecc[1], (size_t)(d.addEccAndInterleave.blockEccLen - 1) * sizeof(d.addEccAndInterleave.ecc[0]));
		d.addEccAndInterleave.ecc[d.addEccAndInterleave.blockEccLen - 1] = 0;
		for (d.reedSolomonComputeRemainder.j = 0; d.reedSolomonComputeRemainder.j < d.addEccAndInterleave.blockEccLen; ++d.reedSolomonComputeRemainder.j)
			d.addEccAndInterleave.ecc[d.reedSolomonComputeRemainder.j] ^= reedSolomonMultiply(d.addEccAndInterleave.rsdiv[d.reedSolomonComputeRemainder.j], d.reedSolomonComputeRemainder.factor);
	}
}

#undef qrcodegen_REED_SOLOMON_DEGREE_MAX





/*---- Drawing function modules ----*/

// Clears the given QR Code grid with light modules for the given
// version's size, then marks every function module as dark.
testable void initializeFunctionModules(uint8_t buf[]) {
	// Initialize QR Code
	int qrsize = version * 4 + 17;
	memset(buf, 0, BUFFER_SIZE);
	buf[0] = (uint8_t)qrsize;
	
	// Fill horizontal and vertical timing patterns
	fillRectangle(6, 0, 1, qrsize, buf);
	fillRectangle(0, 6, qrsize, 1, buf);
	
	// Fill 3 finder patterns (all corners except bottom right) and format bits
	fillRectangle(0, 0, 9, 9, buf);
	fillRectangle(qrsize - 8, 0, 8, 9, buf);
	fillRectangle(0, qrsize - 8, 9, 8, buf);
	
	// Fill numerous alignment patterns
	{
		int numAlign, i, j;
		numAlign = getAlignmentPatternPositions();
		for (i = 0; i < numAlign; i++) {
			for (j = 0; j < numAlign; j++) {
				// Don't draw on the three finder corners
				if (!((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0)))
					fillRectangle(alignPatPos[i] - 2, alignPatPos[j] - 2, 5, 5, buf);
			}
		}
	}
	
	// Fill version blocks
	if (version >= 7) {
		fillRectangle(qrsize - 11, 0, 3, 6, buf);
		fillRectangle(0, qrsize - 11, 6, 3, buf);
	}
}


// Draws light function modules and possibly some dark modules onto the given QR Code, without changing
// non-function modules. This does not draw the format bits. This requires all function modules to be previously
// marked dark (namely by initializeFunctionModules()), because this may skip redrawing dark function modules.
static void drawLightFunctionModules() {
	int i, j, dy, dx;
	// Draw horizontal and vertical timing patterns
	int qrsize = qrcodegen_getSize();
	for (i = 7; i < qrsize - 7; i += 2) {
		setModuleBounded(qrcode, 6, i, false);
		setModuleBounded(qrcode, i, 6, false);
	}
	
	// Draw 3 finder patterns (all corners except bottom right; overwrites some timing modules)
	for (dy = -4; dy <= 4; dy++) {
		for (dx = -4; dx <= 4; dx++) {
			int dist = abs(dx);
			if (abs(dy) > dist)
				dist = abs(dy);
			if (dist == 2 || dist == 4) {
				setModuleUnbounded(3 + dx, 3 + dy, false);
				setModuleUnbounded(qrsize - 4 + dx, 3 + dy, false);
				setModuleUnbounded(3 + dx, qrsize - 4 + dy, false);
			}
		}
	}
	
	// Draw numerous alignment patterns
	{
		int numAlign = getAlignmentPatternPositions();
		for (i = 0; i < numAlign; i++) {
			for (j = 0; j < numAlign; j++) {
				if ((i == 0 && j == 0) || (i == 0 && j == numAlign - 1) || (i == numAlign - 1 && j == 0))
					continue;  // Don't draw on the three finder corners
				for (dy = -1; dy <= 1; dy++) {
					for (dx = -1; dx <= 1; dx++)
						setModuleBounded(qrcode, alignPatPos[i] + dx, alignPatPos[j] + dy, dx == 0 && dy == 0);
				}
			}
		}
	}
	
	// Draw version blocks
	if (version >= 7) {
		uint16_t bits;
		// Calculate error correction code and pack bits
		int rem = version;  // version is uint6, in the range [7, 40]
		for (i = 0; i < 12; i++)
			rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
		bits = (uint16_t)version << 12 | rem;  // uint18
		
		// Draw two copies
		for (i = 0; i < 6; i++) {
			for (j = 0; j < 3; j++) {
				int k = qrsize - 11 + j;
				setModuleBounded(qrcode, k, i, (bits & 1) != 0);
				setModuleBounded(qrcode, i, k, (bits & 1) != 0);
				bits >>= 1;
			}
		}
	}
}


// Draws two copies of the format bits (with its own error correction code) based
// on the given mask and error correction level. This always draws all modules of
// the format bits, unlike drawLightFunctionModules() which might skip dark modules.
static void drawFormatBits(enum qrcodegen_Mask mask) {
	// Calculate error correction code and pack bits
	static const int table[] = {1, 0, 3, 2};
	int data = table[(int)ecl] << 3 | (int)mask;  // errCorrLvl is uint2, mask is uint3
	int rem = data;
	int i, bits, qrsize;
	for (i = 0; i < 10; i++)
		rem = (rem << 1) ^ ((rem >> 9) * 0x537);
	bits = (data << 10 | rem) ^ 0x5412;  // uint15
	
	// Draw first copy
	for (i = 0; i <= 5; i++)
		setModuleBounded(qrcode, 8, i, getBit(bits, i));
	setModuleBounded(qrcode, 8, 7, getBit(bits, 6));
	setModuleBounded(qrcode, 8, 8, getBit(bits, 7));
	setModuleBounded(qrcode, 7, 8, getBit(bits, 8));
	for (i = 9; i < 15; i++)
		setModuleBounded(qrcode, 14 - i, 8, getBit(bits, i));
	
	// Draw second copy
	qrsize = qrcodegen_getSize();
	for (i = 0; i < 8; i++)
		setModuleBounded(qrcode, qrsize - 1 - i, 8, getBit(bits, i));
	for (i = 8; i < 15; i++)
		setModuleBounded(qrcode, 8, qrsize - 15 + i, getBit(bits, i));
	setModuleBounded(qrcode, 8, qrsize - 8, true);  // Always dark
}


// Calculates and stores an ascending list of positions of alignment patterns
// for this version number, returning the length of the list (in the range [0,7]).
// Each position is in the range [0,177), and are used on both the x and y axes.
// This could be implemented as lookup table of 40 variable-length lists of unsigned bytes.
testable int getAlignmentPatternPositions() {
	if (version == 1)
		return 0;
	{
		int numAlign = version / 7 + 2;
		int step = (version == 32) ? 26 :
			(version * 4 + numAlign * 2 + 1) / (numAlign * 2 - 2) * 2;
		int i, pos;
		for (i = numAlign - 1, pos = version * 4 + 10; i >= 1; i--, pos -= step)
			alignPatPos[i] = (uint8_t)pos;
		alignPatPos[0] = 6;
		return numAlign;
	}
}


// Sets every module in the range [left : left + width] * [top : top + height] to dark.
static void fillRectangle(int left, int top, int width, int height, uint8_t buf[]) {
	int dy, dx;
	for (dy = 0; dy < height; dy++) {
		for (dx = 0; dx < width; dx++)
			setModuleBounded(buf, left + dx, top + dy, true);
	}
}



/*---- Drawing data modules and masking ----*/

// Draws the raw codewords (including data and ECC) onto the given QR Code. This requires the initial state of
// the QR Code to be dark at function modules and light at codeword modules (including unused remainder bits).
static void drawCodewords() {
	d.drawCodewords.qrsize = qrcodegen_getSize();
	d.drawCodewords.i = 0;  // Bit index into the data
	// Do the funny zigzag scan
	for (d.drawCodewords.right = d.drawCodewords.qrsize - 1; d.drawCodewords.right != 0 && d.drawCodewords.right != 0xff; d.drawCodewords.right -= 2) {  // Index of right column in each column pair
		if (d.drawCodewords.right == 6)
			d.drawCodewords.right = 5;
		for (d.drawCodewords.vert = 0; d.drawCodewords.vert < d.drawCodewords.qrsize; ++d.drawCodewords.vert) {  // Vertical counter
			for (d.drawCodewords.j = 0; d.drawCodewords.j < 2; ++d.drawCodewords.j) {
				d.drawCodewords.x = d.drawCodewords.right - d.drawCodewords.j;  // Actual x coordinate
				d.drawCodewords.upward = ((d.drawCodewords.right + 1) & 2) == 0; // TODO does a pointless jsr here
				d.drawCodewords.y = d.drawCodewords.upward ? d.drawCodewords.qrsize - 1 - d.drawCodewords.vert : d.drawCodewords.vert;  // Actual y coordinate
				if (!getModuleBounded(qrcode, d.drawCodewords.x, d.drawCodewords.y) && d.drawCodewords.i < d.drawCodewords.datLen * 8) {
					d.drawCodewords.dark = getBit(tempBuffer[d.drawCodewords.i >> 3], 7 - (d.drawCodewords.i & 7));
					setModuleBounded(qrcode, d.drawCodewords.x, d.drawCodewords.y, d.drawCodewords.dark);
					++d.drawCodewords.i;
				}
				// If this QR Code has any remainder bits (0 to 7), they were assigned as
				// 0/false/light by the constructor and are left unchanged by this method
			}
		}
	}
}


// XORs the codeword modules in this QR Code with the given mask pattern
// and given pattern of function modules. The codeword bits must be drawn
// before masking. Due to the arithmetic of XOR, calling applyMask() with
// the same mask value a second time will undo the mask. A final well-formed
// QR Code needs exactly one (not zero, two, etc.) mask applied.
static void applyMask(enum qrcodegen_Mask mask) {
	int qrsize = qrcodegen_getSize();
	int y, x;
	bool invert, val;
	for (y = 0; y < qrsize; y++) {
		for (x = 0; x < qrsize; x++) {
			if (getModuleBounded(tempBuffer, x, y))
				continue;
			switch ((int)mask) {
				case 0:  invert = (x + y) % 2 == 0;                    break;
				case 1:  invert = y % 2 == 0;                          break;
				case 2:  invert = x % 3 == 0;                          break;
				case 3:  invert = (x + y) % 3 == 0;                    break;
				case 4:  invert = (x / 3 + y / 2) % 2 == 0;            break;
				case 5:  invert = x * y % 2 + x * y % 3 == 0;          break;
				case 6:  invert = (x * y % 2 + x * y % 3) % 2 == 0;    break;
				case 7:  invert = ((x + y) % 2 + x * y % 3) % 2 == 0;  break;
			}
			val = getModuleBounded(qrcode, x, y);
			setModuleBounded(qrcode, x, y, val ^ invert);
		}
	}
}


// Calculates and returns the penalty score based on state of the given QR Code's current modules.
// This is used by the automatic mask choice algorithm to find the mask pattern that yields the lowest score.
static uint16_t getPenaltyScore() {
	int qrsize = qrcodegen_getSize();
	uint16_t result = 0;
	
	// Adjacent modules in row having same color, and finder-like patterns
	int y, x, dark;
	for (y = 0; y < qrsize; y++) {
		bool runColor = false;
		int runX = 0;
		int runHistory[7] = {0};
		for (x = 0; x < qrsize; x++) {
			if (getModuleBounded(qrcode, x, y) == runColor) {
				runX++;
				if (runX == 5)
					result += PENALTY_N1;
				else if (runX > 5)
					result++;
			} else {
				finderPenaltyAddHistory(runX, runHistory, qrsize);
				if (!runColor)
					result += finderPenaltyCountPatterns(runHistory) * PENALTY_N3;
				runColor = getModuleBounded(qrcode, x, y);
				runX = 1;
			}
		}
		result += finderPenaltyTerminateAndCount(runColor, runX, runHistory, qrsize) * PENALTY_N3;
	}
	// Adjacent modules in column having same color, and finder-like patterns
	for (x = 0; x < qrsize; x++) {
		bool runColor = false;
		int runY = 0;
		int runHistory[7] = {0};
		for (y = 0; y < qrsize; y++) {
			if (getModuleBounded(qrcode, x, y) == runColor) {
				runY++;
				if (runY == 5)
					result += PENALTY_N1;
				else if (runY > 5)
					result++;
			} else {
				finderPenaltyAddHistory(runY, runHistory, qrsize);
				if (!runColor)
					result += finderPenaltyCountPatterns(runHistory) * PENALTY_N3;
				runColor = getModuleBounded(qrcode, x, y);
				runY = 1;
			}
		}
		result += finderPenaltyTerminateAndCount(runColor, runY, runHistory, qrsize) * PENALTY_N3;
	}
	
	// 2*2 blocks of modules having same color
	for (y = 0; y < qrsize - 1; y++) {
		for (x = 0; x < qrsize - 1; x++) {
			bool  color = getModuleBounded(qrcode, x, y);
			if (  color == getModuleBounded(qrcode, x + 1, y) &&
			      color == getModuleBounded(qrcode, x, y + 1) &&
			      color == getModuleBounded(qrcode, x + 1, y + 1))
				result += PENALTY_N2;
		}
	}
	
	// Balance of dark and light modules
	dark = 0;
	for (y = 0; y < qrsize; y++) {
		for (x = 0; x < qrsize; x++) {
			if (getModuleBounded(qrcode, x, y))
				dark++;
		}
	}
	{
		int total = qrsize * qrsize;  // Note that size is odd, so dark/total != 1/2
		// Compute the smallest integer k >= 0 such that (45-5k)% <= dark/total <= (55+5k)%
		int k = (int)((labs(dark * 20L - total * 10L) + total - 1) / total) - 1;
		result += k * PENALTY_N4;
		return result;
	}
}


// Can only be called immediately after a light run is added, and
// returns either 0, 1, or 2. A helper function for getPenaltyScore().
static uint8_t finderPenaltyCountPatterns(const int runHistory[7]) {
	int n = runHistory[1];
	bool core = n > 0 && runHistory[2] == n && runHistory[3] == n * 3 && runHistory[4] == n && runHistory[5] == n;
	// The maximum QR Code size is 177, hence the dark run length n <= 177.
	// Arithmetic is promoted to int, so n*4 will not overflow.
	return (core && runHistory[0] >= n * 4 && runHistory[6] >= n ? 1 : 0)
	     + (core && runHistory[6] >= n * 4 && runHistory[0] >= n ? 1 : 0);
}


// Must be called at the end of a line (row or column) of modules. A helper function for getPenaltyScore().
static int finderPenaltyTerminateAndCount(bool currentRunColor, int currentRunLength, int runHistory[7], int qrsize) {
	if (currentRunColor) {  // Terminate dark run
		finderPenaltyAddHistory(currentRunLength, runHistory, qrsize);
		currentRunLength = 0;
	}
	currentRunLength += qrsize;  // Add light border to final run
	finderPenaltyAddHistory(currentRunLength, runHistory, qrsize);
	return finderPenaltyCountPatterns(runHistory);
}


// Pushes the given value to the front and drops the last value. A helper function for getPenaltyScore().
static void finderPenaltyAddHistory(int currentRunLength, int runHistory[7], int qrsize) {
	if (runHistory[0] == 0)
		currentRunLength += qrsize;  // Add light border to initial run
	memmove(&runHistory[1], &runHistory[0], 6 * sizeof(runHistory[0]));
	runHistory[0] = currentRunLength;
}



/*---- Basic QR Code information ----*/

// Public function - see documentation comment in header file.
uint8_t qrcodegen_getSize() {
	uint8_t result = qrcode[0];
	return result;
}


// Public function - see documentation comment in header file.
bool qrcodegen_getModule(int x, int y) {
	int qrsize = qrcode[0];
	return (0 <= x && x < qrsize && 0 <= y && y < qrsize) && getModuleBounded(qrcode, x, y);
}


// Returns the color of the module at the given coordinates, which must be in bounds.
testable bool getModuleBounded(const uint8_t buf[], int8_t x, int8_t y) {
	d.getModuleBounded.index = y * BUFFER_WIDTH + x;
	return getBit(buf[(d.getModuleBounded.index >> 3) + 1], d.getModuleBounded.index & 7);
}


// Sets the color of the module at the given coordinates, which must be in bounds.
testable void setModuleBounded(uint8_t buf[], int8_t x, int8_t y, bool isDark) {
	static const uint8_t shifts[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	{
		d.setModuleBounded.index = y * BUFFER_WIDTH + x;
		d.setModuleBounded.bitIndex = d.setModuleBounded.index & 7;
		d.setModuleBounded.byteIndex = (d.setModuleBounded.index >> 3) + 1;
		if (isDark)
			buf[d.setModuleBounded.byteIndex] |= shifts[d.setModuleBounded.bitIndex];
		else
			buf[d.setModuleBounded.byteIndex] &= shifts[d.setModuleBounded.bitIndex] ^ 0xFF;
	}
}


// Sets the color of the module at the given coordinates, doing nothing if out of bounds.
testable void setModuleUnbounded(int8_t x, int8_t y, bool isDark) {
	uint8_t qrsize = qrcode[0];
	if (0 <= x && x < qrsize && 0 <= y && y < qrsize)
		setModuleBounded(qrcode, x, y, isDark);
}


// Returns true iff the i'th bit of x is set to 1. Requires x >= 0 and 0 <= i <= 14.
static bool fastcall getBit(int x, uint8_t i) {
	return (x >> i) & 1;
}



/*---- Segment handling ----*/

// Calculates the number of bits needed to encode the given segments at the given version.
// Returns a non-negative number if successful. Otherwise returns LENGTH_OVERFLOW if a segment
// has too many characters to fit its length field, or the total bits exceeds INT16_MAX.
testable uint16_t getTotalBits() {
	uint8_t ccbits = numCharCountBits();
	if (ccbits == 8 && dataLen >= 256)
	{
		return LENGTH_OVERFLOW;
	}
	return 4 + ccbits + bitLength;
}


// Returns the bit width of the character count field for a segment in the given mode
// in a QR Code at the given version number. The result is in the range [0, 16].
static uint8_t numCharCountBits() {
	return version < 10 ? 8 : 16;
}


#undef LENGTH_OVERFLOW