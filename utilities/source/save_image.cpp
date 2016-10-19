// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <png.h>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include "glurg/common/pixelData.hpp"
#include "glurg/common/process.hpp"
#include "glurg/resources/renderState.hpp"
#include "glurg/resources/resourceFingerprint.hpp"
#include "glurg/resources/texture/textureResource.hpp"
#include "glurg/resources/texture/textureResourceBlob.hpp"
#include "glurg/resources/texture/textureResourceBlobBuilder.hpp"

int main(int argc, const char* argv[])
{
	if (argc < 4)
	{
		std::cerr << argv[1] << " <file.trace> <call-no> <output.png>" << 
			std::endl;

		return 1;
	}

	try
	{
		glurg::Process process;
		process.set_program_name("glretrace");
		process.push_program_argument("-D");
		process.push_program_argument(argv[2]);
		process.push_program_argument("--dump-format");
		process.push_program_argument("json");
		process.push_program_argument(argv[1]);
		process.open(glurg::Process::mode_read);

		glurg::RenderState renderState;
		renderState.populate(process.get_stream());

		glurg::TextureResourceBlobBuilder builder;
		builder.set_binding_point(0);
		builder.set_texture_type(glurg::TextureResourceBlob::type_2d);
		builder.set_mipmap_level(0);
		builder.extract(renderState);

		auto blob = builder.build();
		glurg::TextureResource resource(blob);

		auto fingerprint = resource.get_fingerprint().to_string();
		std::cout << "image fingerprint: " << fingerprint << std::endl;

		glurg::PixelDataBuffer buffer;
		resource.decode_image(buffer);

		png_image image = { 0 };
		image.version = PNG_IMAGE_VERSION;
		image.width = blob->get_width();
		image.height = blob->get_height();
		image.format = PNG_FORMAT_RGBA;

		png_image_write_to_file(
			&image, argv[3], false, 
			&buffer[0], blob->get_width() * 4, nullptr);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;

		return 1;
	}

	return 0;
}
