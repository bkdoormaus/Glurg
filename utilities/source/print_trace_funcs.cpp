// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include "glurg/common/simpleFileStream.hpp"
#include "glurg/common/snappyAdapter.hpp"
#include "glurg/trace/callSignature.hpp"
#include "glurg/trace/events.hpp"
#include "glurg/trace/traceFile.hpp"
#include "glurg/trace/values.hpp"

int main(int argc, const char* argv[])
{
	if (argc < 2)
	{
		std::cout << argv[1] << " <file.trace>" << std::endl;

		return 1;
	}

	try
	{
		glurg::SnappyAdapter<glurg::SimpleFileStream> fileStream;
		fileStream.open(argv[1], glurg::FileStream::mode_read);

		glurg::TraceFile traceFile;
		traceFile.verify_is_compatible_version(fileStream);

		while (!fileStream.get_is_end_of_file())
		{
			glurg::Event* e = glurg::Event::read(traceFile, fileStream);
			if (e->get_type() == glurg::Event::event_enter)
			{
				std::cout << "enter #" << e->get_call_index();
			}
			else if (e->get_type() == glurg::Event::event_leave)
			{
				std::cout << "leave #" << e->get_call_index();
			}
			
			glurg::Call* call = traceFile.get_call(e->get_call_index());
			std::cout << ": " << call->get_call_signature()->get_name();
			std::cout << std::endl;

			if (e->get_type() == glurg::Event::event_leave)
			{
				traceFile.delete_call(traceFile.get_call(e->get_call_index()));
			}

			delete e;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;

		return 1;
	}
}
