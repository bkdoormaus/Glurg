// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/resources/texture/pixelUnpacker.hpp"

void glurg::PixelUnpacker::fetch_pixel(
	const std::uint8_t* pixels,
	int row, int row_length, int column,
	UnpackedPixel& pixel) const
{
	auto r = fetch_row(pixels, row_length, row);
	fetch_pixel(r, column, pixel);
}

template <typename PixelComponent>
class IntegerPixelUnpacker : public glurg::PixelUnpacker
{
public:
	// Size of a pixel, in bytes.
	static const std::size_t PIXEL_SIZE = sizeof(PixelComponent);
	typedef PixelComponent Component;

	const std::uint8_t* fetch_row(
		const std::uint8_t* pixels, int row_length, int row) const override;

	static void normalize_pixel(
		glurg::UnpackedPixel& pixel,
		float red, float green, float blue, float alpha);
};

template <typename PixelComponent>
const std::uint8_t* IntegerPixelUnpacker<PixelComponent>::fetch_row(
	const std::uint8_t* pixels, int row_length, int row) const
{
	std::uint32_t row_size = (std::uint32_t)row_length * PIXEL_SIZE;
	return pixels + ((std::uint32_t)row * row_size);
}

template <typename PixelComponent>
void IntegerPixelUnpacker<PixelComponent>::normalize_pixel(
		glurg::UnpackedPixel& pixel,
		float red, float green, float blue, float alpha)
{
	pixel.red = (std::uint8_t)(pixel.red * red * 255);
	pixel.green = (std::uint8_t)(pixel.green * green * 255);
	pixel.blue = (std::uint8_t)(pixel.blue * blue * 255);
	pixel.alpha = (std::uint8_t)(pixel.alpha * alpha * 255);
}

class PixelUnpacker_UnsignedByte_3_3_2 :
	public IntegerPixelUnpacker<std::uint8_t>
{
public:
	void fetch_pixel(
		const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const;
};

void PixelUnpacker_UnsignedByte_3_3_2::fetch_pixel(
	const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const
{
	const Component MASK_3BITS = (1 << 4) - 1;
	const Component MASK_2BITS = (1 << 3) - 1;

	auto value = row[column];
	pixel.red = value & MASK_3BITS;
	pixel.green = (value >> 3) & MASK_3BITS;
	pixel.blue = (value >> 6) & MASK_2BITS;
	pixel.alpha = 0;

	normalize_pixel(pixel,
		1.0f / MASK_3BITS, 1.0f / MASK_3BITS, 1.0f / MASK_2BITS, 0.0f);
}

class PixelUnpacker_UnsignedByte_2_3_3_REV :
	public IntegerPixelUnpacker<std::uint8_t>
{
public:
	void fetch_pixel(
		const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const;
};

void PixelUnpacker_UnsignedByte_2_3_3_REV::fetch_pixel(
	const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const
{
	const Component MASK_3BITS = (1 << 4) - 1;
	const Component MASK_2BITS = (1 << 3) - 1;

	auto value = row[column];
	pixel.red = (value >> 5) & MASK_3BITS;
	pixel.green = (value >> 2) & MASK_3BITS;
	pixel.blue = value & MASK_2BITS;
	pixel.alpha = 0;

	normalize_pixel(pixel,
		1.0f / MASK_3BITS, 1.0f / MASK_3BITS, 1.0f / MASK_2BITS, 0.0f);
}

class PixelUnpacker_UnsignedShort_5_6_5 :
	public IntegerPixelUnpacker<std::uint16_t>
{
public:
	void fetch_pixel(
		const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const;
};

void PixelUnpacker_UnsignedShort_5_6_5::fetch_pixel(
	const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const
{
	const Component MASK_6BITS = (1 << 7) - 1;
	const Component MASK_5BITS = (1 << 6) - 1;

	Component value = ((Component*)row)[column];
	pixel.red = value & MASK_5BITS;
	pixel.green = (value >> 5) & MASK_6BITS;
	pixel.blue = (value >> 11) & MASK_5BITS;
	pixel.alpha = 0;

	normalize_pixel(pixel,
		1.0f / MASK_5BITS, 1.0f / MASK_6BITS, 1.0f / MASK_5BITS, 0.0f);
}

class PixelUnpacker_UnsignedShort_5_6_5_REV :
	public IntegerPixelUnpacker<std::uint16_t>
{
public:
	void fetch_pixel(
		const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const;
};

void PixelUnpacker_UnsignedShort_5_6_5_REV::fetch_pixel(
	const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const
{
	const Component MASK_6BITS = (1 << 7) - 1;
	const Component MASK_5BITS = (1 << 6) - 1;

	Component value = ((Component*)row)[column];
	pixel.red = (value >> 11) & MASK_5BITS;
	pixel.green = (value >> 5) & MASK_6BITS;
	pixel.blue = value & MASK_5BITS;
	pixel.alpha = 0;

	normalize_pixel(pixel,
		1.0f / MASK_5BITS, 1.0f / MASK_6BITS, 1.0f / MASK_5BITS, 0.0f);
}

class PixelUnpacker_UnsignedShort_4_4_4_4 :
	public IntegerPixelUnpacker<std::uint16_t>
{
public:
	void fetch_pixel(
		const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const;
};

void PixelUnpacker_UnsignedShort_4_4_4_4::fetch_pixel(
	const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const
{
	const Component MASK_4BITS = (1 << 4) - 1;

	Component value = ((Component*)row)[column];
	pixel.red = value & MASK_4BITS;
	pixel.green = (value >> 4) & MASK_4BITS;
	pixel.blue = (value >> 8) & MASK_4BITS;
	pixel.alpha = (value >> 12) & MASK_4BITS;

	normalize_pixel(
		pixel,
		1.0f / MASK_4BITS,
		1.0f / MASK_4BITS,
		1.0f / MASK_4BITS,
		1.0f / MASK_4BITS);
}

class PixelUnpacker_UnsignedShort_4_4_4_4_REV :
	public IntegerPixelUnpacker<std::uint16_t>
{
public:
	void fetch_pixel(
		const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const;
};

void PixelUnpacker_UnsignedShort_4_4_4_4_REV::fetch_pixel(
	const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const
{
	const Component MASK_4BITS = (1 << 4) - 1;

	Component value = ((Component*)row)[column];
	pixel.red = (value >> 12) & MASK_4BITS;
	pixel.green = (value >> 8) & MASK_4BITS;
	pixel.blue = (value >> 4) & MASK_4BITS;
	pixel.alpha = value & MASK_4BITS;

	normalize_pixel(
		pixel,
		1.0f / MASK_4BITS,
		1.0f / MASK_4BITS,
		1.0f / MASK_4BITS,
		1.0f / MASK_4BITS);
}

class PixelUnpacker_UnsignedShort_5_5_5_1 :
	public IntegerPixelUnpacker<std::uint16_t>
{
public:
	void fetch_pixel(
		const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const;
};

void PixelUnpacker_UnsignedShort_5_5_5_1::fetch_pixel(
	const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const
{
	const Component MASK_5BITS = (1 << 6) - 1;
	const Component MASK_1BIT = 1;

	Component value = ((Component*)row)[column];
	pixel.red = value & MASK_5BITS;
	pixel.green = (value >> 5) & MASK_5BITS;
	pixel.blue = (value >> 10) & MASK_5BITS;
	pixel.alpha = (value >> 15) & MASK_1BIT;

	normalize_pixel(
		pixel,
		1.0f / MASK_5BITS,
		1.0f / MASK_5BITS,
		1.0f / MASK_5BITS,
		1.0f);
}

class PixelUnpacker_UnsignedShort_5_5_5_1_REV :
	public IntegerPixelUnpacker<std::uint16_t>
{
public:
	void fetch_pixel(
		const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const;
};

void PixelUnpacker_UnsignedShort_5_5_5_1_REV::fetch_pixel(
	const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const
{
	const Component MASK_5BITS = (1 << 6) - 1;
	const Component MASK_1BIT = 1;

	Component value = ((Component*)row)[column];
	pixel.red = (value >> 11) & MASK_5BITS;
	pixel.green = (value >> 6) & MASK_5BITS;
	pixel.blue = (value >> 1) & MASK_5BITS;
	pixel.alpha = value & MASK_1BIT;

	normalize_pixel(
		pixel,
		1.0f / MASK_5BITS,
		1.0f / MASK_5BITS,
		1.0f / MASK_5BITS,
		1.0f);
}

class PixelUnpacker_UnsignedInt_8_8_8_8 :
	public IntegerPixelUnpacker<std::uint32_t>
{
public:
	void fetch_pixel(
		const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const;
};

void PixelUnpacker_UnsignedInt_8_8_8_8::fetch_pixel(
	const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const
{
	const Component MASK_8BITS = (1 << 9) - 1;

	Component value = ((Component*)row)[column];
	pixel.red = value & MASK_8BITS;
	pixel.green = (value >> 8) & MASK_8BITS;
	pixel.blue = (value >> 16) & MASK_8BITS;
	pixel.alpha = (value >> 24) & MASK_8BITS;
}

class PixelUnpacker_UnsignedInt_8_8_8_8_REV :
	public IntegerPixelUnpacker<std::uint32_t>
{
public:
	void fetch_pixel(
		const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const;
};

void PixelUnpacker_UnsignedInt_8_8_8_8_REV::fetch_pixel(
	const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const
{
	const Component MASK_8BITS = (1 << 9) - 1;

	Component value = ((Component*)row)[column];
	pixel.red = (value >> 24) & MASK_8BITS;
	pixel.green = (value >> 16) & MASK_8BITS;
	pixel.blue = (value >> 8) & MASK_8BITS;
	pixel.alpha = value & MASK_8BITS;
}

class PixelUnpacker_UnsignedInt_10_10_10_2 :
	public IntegerPixelUnpacker<std::uint32_t>
{
public:
	void fetch_pixel(
		const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const;
};

void PixelUnpacker_UnsignedInt_10_10_10_2::fetch_pixel(
	const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const
{
	const Component MASK_10BITS = (1 << 11) - 1;
	const Component MASK_2BITS = (1 << 3) - 1;

	Component value = ((Component*)row)[column];
	pixel.red = value & MASK_10BITS;
	pixel.green = (value >> 10) & MASK_10BITS;
	pixel.blue = (value >> 20) & MASK_10BITS;
	pixel.alpha = (value >> 30) & MASK_2BITS;

	normalize_pixel(
		pixel,
		1.0f / MASK_10BITS,
		1.0f / MASK_10BITS,
		1.0f / MASK_10BITS,
		1.0f / MASK_2BITS);
}

class PixelUnpacker_UnsignedInt_10_10_10_2_REV :
	public IntegerPixelUnpacker<std::uint32_t>
{
public:
	void fetch_pixel(
		const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const;
};

void PixelUnpacker_UnsignedInt_10_10_10_2_REV::fetch_pixel(
	const std::uint8_t* row, int column, glurg::UnpackedPixel& pixel) const
{
	const Component MASK_10BITS = (1 << 11) - 1;
	const Component MASK_2BITS = (1 << 3) - 1;

	Component value = ((Component*)row)[column];
	pixel.alpha = value & MASK_2BITS;
	pixel.blue = (value >> 2) & MASK_10BITS;
	pixel.green = (value >> 12) & MASK_10BITS;
	pixel.red = (value >> 22) & MASK_10BITS;

	normalize_pixel(
		pixel,
		1.0f / MASK_10BITS,
		1.0f / MASK_10BITS,
		1.0f / MASK_10BITS,
		1.0f / MASK_2BITS);
}

const glurg::PixelUnpacker* glurg::PixelUnpacker::get_pixel_fetcher(
	GLenum format)
{
	switch (format)
	{
		case GL_UNSIGNED_BYTE_3_3_2:
			{
				static PixelUnpacker_UnsignedByte_3_3_2 fetcher;
				return &fetcher;
			}
		case GL_UNSIGNED_BYTE_2_3_3_REV:
			{
				static PixelUnpacker_UnsignedByte_2_3_3_REV fetcher;
				return &fetcher;
			}
		case GL_UNSIGNED_SHORT_5_6_5:
			{
				static PixelUnpacker_UnsignedShort_5_6_5 fetcher;
				return &fetcher;
			}
		case GL_UNSIGNED_SHORT_5_6_5_REV:
			{
				static PixelUnpacker_UnsignedShort_5_6_5_REV fetcher;
				return &fetcher;
			}
		case GL_UNSIGNED_SHORT_4_4_4_4:
			{
				static PixelUnpacker_UnsignedShort_4_4_4_4 fetcher;
				return &fetcher;
			}
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
			{
				static PixelUnpacker_UnsignedShort_4_4_4_4_REV fetcher;
				return &fetcher;
			}
		case GL_UNSIGNED_SHORT_5_5_5_1:
			{
				static PixelUnpacker_UnsignedShort_5_5_5_1 fetcher;
				return &fetcher;
			}
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
			{
				static PixelUnpacker_UnsignedShort_5_5_5_1 fetcher;
				return &fetcher;
			}
		case GL_UNSIGNED_INT_8_8_8_8:
			{
				static PixelUnpacker_UnsignedInt_8_8_8_8 fetcher;
				return &fetcher;
			}
		case GL_UNSIGNED_INT_8_8_8_8_REV:
			{
				static PixelUnpacker_UnsignedInt_8_8_8_8_REV fetcher;
				return &fetcher;
			}
		case GL_UNSIGNED_INT_10_10_10_2:
			{
				static PixelUnpacker_UnsignedInt_10_10_10_2 fetcher;
				return &fetcher;
			}
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			{
				static PixelUnpacker_UnsignedInt_10_10_10_2_REV fetcher;
				return &fetcher;
			}
		default:
			return nullptr;
	}
}
