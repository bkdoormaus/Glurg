// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_VALUES_HPP
#define LIBGLURG_TRACE_VALUES_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace glurg
{
	class Array;
	class Bitmask;
	class BitmaskSignature;
	class Enumeration;
	class EnumerationSignature;
	class FileStream;
	class TraceFile;

	class Value
	{
	public:
		typedef std::uint8_t Type;
		typedef Value* (* ReadFunction)(Type, TraceFile&, FileStream&);

		virtual Type get_type() const = 0;

		virtual bool to_boolean() const;

		virtual std::int32_t to_signed_integer() const;
		virtual std::uint32_t to_unsigned_integer() const;

		virtual float to_single() const;
		virtual double to_double() const;

		virtual std::string to_string() const;

		virtual const std::uint8_t* to_pointer() const;
		virtual std::uint32_t to_handle() const;

		virtual const Enumeration* to_enumeration() const;
		virtual const Bitmask* to_bitmask() const;
		virtual const Array* to_array() const;

		virtual Value* clone() const = 0;
	};

	struct Enumeration
	{
		EnumerationSignature* signature;
		Value* value;
	};

	struct Bitmask
	{
		BitmaskSignature* signature;
		std::uint32_t value;
	};

	class Array
	{
	public:
		std::size_t get_size() const;
		void set_size(std::size_t value);

		const Value* get_value_at(std::size_t index) const;
		void set_value_at(std::size_t index, const Value* value);

	private:
		typedef std::unique_ptr<Value> ValuePointer;
		std::vector<ValuePointer> values;
	};

	class BoolValue : public Value
	{
	public:
		Type get_type() const override;
		Value* clone() const override;

		bool to_boolean() const override;

		static Value* read_false_boolean(
			Type type, TraceFile& trace, FileStream& stream);
		static Value* read_true_boolean(
			Type type, TraceFile& trace, FileStream& stream);

	private:
		bool value;
	};

	class IntegerValue : public Value
	{
	public:
		Type get_type() const override;
		Value* clone() const override;
		
		std::int32_t to_signed_integer() const override;
		std::uint32_t to_unsigned_integer() const override;

		static Value* read_positive_integer(
			Type type, TraceFile& trace, FileStream& stream);
		static Value* read_negative_integer(
			Type type, TraceFile& trace, FileStream& stream);

	private:
		std::int64_t value;
	};

	class FloatingPointValue : public Value
	{
	public:
		Type get_type() const override;
		Value* clone() const override;
		
		float to_single() const override;
		double to_double() const override;

		static Value* read_single(
			Type type, TraceFile& trace, FileStream& stream);
		static Value* read_double(
			Type type, TraceFile& trace, FileStream& stream);

	private:
		double value;
	};

	class StringValue : public Value
	{
	public:
		Type get_type() const override;
		Value* clone() const override;
		
		std::string to_string() const override;
		const std::uint8_t* to_pointer() const override;

		static Value* read_string(
			Type type, TraceFile& trace, FileStream& stream);

	private:
		std::string value;
	};

	class BlobValue : public Value
	{
	public:
		Type get_type() const override;
		Value* clone() const override;

		const std::uint8_t* to_pointer() const override;

		static Value* read_blob(
			Type type, TraceFile& trace, FileStream& stream);
	private:
		std::uint8_t* value;
	};

	class EnumerationValue : public Value
	{
	public:
		Type get_type() const override;
		Value* clone() const override;

		const Enumeration* to_enumeration() const override;

		static Value* read_enumeration(
			Type type, TraceFile& trace, FileStream& stream);

	private:
		Enumeration value;
	};

	class BitmaskValue : public Value
	{
	public:
		Type get_type() const override;
		Value* clone() const override;

		const Bitmask* to_bitmask() const override;

		static Value* read_bitmask(
			Type type, TraceFile& trace, FileStream& stream);

	private:
		Bitmask value;
	};

	class ArrayValue : public Value
	{
	public:
		Type get_type() const override;
		Value* clone() const override;

		const Array* to_array() const override;

		static Value* read_array(
			Type type, TraceFile& trace, FileStream& stream);
	private:
		Array value;
	};

	class HandleValue : public Value
	{
	public:
		Type get_type() const override;
		Value* clone() const override;

		std::uint32_t to_handle() const override;

		static Value* read_handle(
			Type type, TraceFile& trace, FileStream& stream);

	private:
		std::uint32_t value;
	};
}

#endif
