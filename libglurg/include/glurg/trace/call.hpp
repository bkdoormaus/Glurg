// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_CALL_HPP
#define LIBGLURG_TRACE_CALL_HPP

#include <cstdint>
#include <vector>

namespace glurg
{
	class CallSignature;
	class Value;

	class Call
	{
	public:
		typedef std::uint32_t ArgumentCount;
		typedef std::uint32_t ArgumentIndex;
		typedef std::uint32_t Index;
		typedef std::uint32_t Thread;

		Call(Index index, const CallSignature* signature);
		~Call() = default;

		Index get_call_index() const;
		const CallSignature* get_call_signature() const;

		Thread get_thread() const;
		void set_thread(Thread thread);

		const Value* get_argument_at(ArgumentIndex index) const;
		void set_argument_at(ArgumentIndex index, const Value* value);

		const Value* get_return_value() const;
		void set_return_value(const Value* value);

	private:
		Index index;
		const CallSignature* signature;

		Thread thread;

		typedef std::unique_ptr<Value> ValuePointer;
		std::vector<ValuePointer> arguments;

		ValuePointer return_value;
	};
}

#endif
