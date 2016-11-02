#ifndef LIBGLURG_DEP_TEXTURE_COMPRESSION_H
#define LIBGLURG_DEP_TEXTURE_COMPRESSION_H

#ifdef __cplusplus
extern "C" {
#endif

enum BC4Mode
{
	BC4_UNORM = 0,
	BC4_SNORM = 1
};

enum BC5Mode
{
	BC5_UNORM = 0,
	BC5_SNORM = 1
};

void DecompressBlockBC1 (uint32_t x, uint32_t y, uint32_t stride,
	const uint8_t* blockStorage, unsigned char* image);
void DecompressBlockBC2 (uint32_t x, uint32_t y, uint32_t stride,
	const uint8_t* blockStorage, unsigned char* image);
void DecompressBlockBC3 (uint32_t x, uint32_t y, uint32_t stride,
	const uint8_t* blockStorage, unsigned char* image);
void DecompressBlockBC4 (uint32_t x, uint32_t y, uint32_t stride, enum BC4Mode mode,
	const uint8_t* blockStorage, unsigned char* image);
void DecompressBlockBC5 (uint32_t x, uint32_t y, uint32_t stride, enum BC5Mode mode,
	const uint8_t* blockStorage, unsigned char* image);

#ifdef __cplusplus
}
#endif

#endif
