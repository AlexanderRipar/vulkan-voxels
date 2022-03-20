#pragma once

#include <cstdint>

#include "texels.h"

#include "och_fio.h"

#pragma pack(push, 1)
struct bitmap_header
{
	//Header
	char header_field[2];
	uint32_t file_size_bytes;
	uint16_t application_specific_1;
	uint16_t application_specific_2;
	uint32_t image_offset;

	//DIB (Info-Header)
	uint32_t header_bytes;
	int32_t width;
	int32_t height;
	uint16_t colour_planes;
	uint16_t bits_per_pixel;
	uint32_t compression_method;
	uint32_t image_size;
	uint32_t horizontal_resolution;
	uint32_t vertical_resolution;
	uint32_t colour_palette_count;
	uint32_t important_colour_count;

	uint8_t* raw_image_data() noexcept
	{
		return { reinterpret_cast<uint8_t*>(this) + image_offset };
	}

	static uint32_t allocation_size(uint32_t image_offset, uint32_t width, uint32_t height) noexcept
	{
		return image_offset + height * stride(width);
	}

	static uint32_t stride(uint32_t width) noexcept
	{
		return (width * 3 + 3) & ~3;
	}

	void initialize(uint32_t file_sz, uint32_t image_data_offset, uint32_t image_width, uint32_t image_height) noexcept
	{
		header_field[0] = 'B';
		header_field[1] = 'M';
		file_size_bytes = file_sz;
		application_specific_1 = 0;
		application_specific_2 = 0;
		image_offset = image_data_offset;
		header_bytes = 40;
		width = image_width;
		height = image_height;
		colour_planes = 1;
		bits_per_pixel = 24;
		compression_method = 0;
		image_size = 0;
		horizontal_resolution = 3780;
		vertical_resolution = 3780;
		colour_palette_count = 0;
		important_colour_count = 0;
	}
};
#pragma pack(pop)

struct bitmap_file
{
private:

	static constexpr uint32_t image_data_offset = 64;

	static_assert(image_data_offset >= sizeof(bitmap_header));

	och::mapped_file<bitmap_header> m_file;

	uint8_t* m_image_data = nullptr;

	uint32_t m_stride = 0;

	uint32_t m_width = 0;

	uint32_t m_height = 0;

public:

	using point_op_fn = texel_b8g8r8(*) (texel_b8g8r8) noexcept;

	och::status create(const char* filename, och::fio::open existing_mode = och::fio::open::normal, uint32_t new_width = 0, uint32_t new_height = 0) noexcept
	{
		const och::fio::open new_mode = new_width && new_height ? och::fio::open::normal : och::fio::open::fail;

		const uint32_t mapping_size = new_width && new_height ? bitmap_header::stride(new_width) * new_height * 3 + image_data_offset : 0;

		check(m_file.create(filename, och::fio::access::read_write, existing_mode, new_mode, 0, mapping_size));

		if (m_file[0].image_offset)
		{
			m_image_data = m_file[0].raw_image_data();

			m_stride = bitmap_header::stride(m_file[0].width);

			m_width = m_file[0].width;

			m_height = m_file[0].height;
		}
		else
		{
			m_image_data = reinterpret_cast<uint8_t*>(m_file.data()) + image_data_offset;

			m_stride = bitmap_header::stride(new_width);

			m_width = new_width;

			m_height = new_height;

			m_file[0].initialize(m_stride * m_height + image_data_offset, image_data_offset, m_width, m_height);
		}

		return {};
	}

	void destroy() noexcept
	{
		m_file.close();

		m_image_data = nullptr;
	}

	texel_b8g8r8& operator()(uint32_t x, uint32_t y) noexcept
	{
		return reinterpret_cast<texel_b8g8r8*>(m_image_data + y * m_stride)[x];
	}

	const texel_b8g8r8& operator()(uint32_t x, uint32_t y) const noexcept
	{
		return reinterpret_cast<const texel_b8g8r8*>(m_image_data + y * m_stride)[x];
	}

	uint32_t width() const noexcept
	{
		return m_width;
	}

	uint32_t height() const noexcept
	{
		return m_height;
	}

	uint8_t* raw_image_data() noexcept
	{
		return m_image_data;
	}

	const uint8_t* raw_image_data() const noexcept
	{
		return m_image_data;
	}

	void point_op(point_op_fn operation) noexcept
	{
		for (uint32_t y = 0; y != m_height; ++y)
			for (uint32_t x = 0; x != m_width; ++x)
				operator()(x, y) = operation(operator()(x, y));
	}
};
