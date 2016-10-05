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
	class BitmaskSignature;
	class EnumerationSignature;
	class FileStream;
	class Structure;
	class StructureSignature;
	class TraceFile;
	class Value;

	struct Enumeration
	{
		const EnumerationSignature* signature;
		Value* value;
	};

	struct Bitmask
	{
		const BitmaskSignature* signature;
		std::uint32_t value;
	};

	struct Blob
	{
		const std::uint8_t* data;
		std::size_t length;
	};

	class Value
	{
	public:
		typedef std::uint8_t Type;
		typedef Value* (* ReadFunction)(Type, TraceFile&, FileStream&);

		virtual ~Value() = default;

		virtual Type get_type() const = 0;

		virtual bool to_boolean() const;

		virtual std::int32_t to_signed_integer() const;
		virtual std::uint32_t to_unsigned_integer() const;

		virtual float to_single() const;
		virtual double to_double() const;

		virtual std::string to_string() const;

		virtual Blob to_blob() const;
		virtual std::uint32_t to_handle() const;

		virtual Enumeration to_enumeration() const;
		virtual Bitmask to_bitmask() const;
		virtual const Array* to_array() const;
		virtual const Structure* to_structure() const;

		virtual Value* clone() const = 0;
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
		static const Type FALSE_BOOLEAN = 0x01;
		static const Type TRUE_BOOLEAN = 0x02;

		BoolValue() = default;
		explicit BoolValue(bool value);

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
		static const Type NEGATIVE_NUMBER = 0x03;
		static const Type POSITIVE_NUMBER = 0x04;

		IntegerValue() = default;
		explicit IntegerValue(std::int64_t value);

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
		static const Type FLOAT = 0x05;
		static const Type DOUBLE = 0x06;

		FloatingPointValue() = default;
		FloatingPointValue(Type type, double value);

		Type get_type() const override;
		Value* clone() const override;
		
		float to_single() const override;
		double to_double() const override;

		static Value* read_single(
			Type type, TraceFile& trace, FileStream& stream);
		static Value* read_double(
			Type type, TraceFile& trace, FileStream& stream);

	private:
		Type type;
		double value;
	};

	class StringValue : public Value
	{
	public:
		static const Type STRING = 0x07;

		StringValue() = default;
		StringValue(const std::string& value);

		Type get_type() const override;
		Value* clone() const override;
		
		std::string to_string() const override;
		Blob to_blob() const override;

		static Value* read_string(
			Type type, TraceFile& trace, FileStream& stream);

	private:
		std::string value;
	};

	class BlobValue : public Value
	{
	public:
		static const Type NULL_POINTER = 0x00;
		static const Type BLOB = 0x08;

		BlobValue() = default;
		explicit BlobValue(std::size_t length);
		BlobValue(std::size_t length, const std::uint8_t* data);
		~BlobValue();

		Type get_type() const override;
		Value* clone() const override;

		Blob to_blob() const override;

		static Value* read_blob(
			Type type, TraceFile& trace, FileStream& stream);
		static Value* read_null_pointer(
			Type type, TraceFile& trace, FileStream& stream);
	private:
		std::uint8_t* data;
		std::size_t length;
	};

	class EnumerationValue : public Value
	{
	public:
		static const Type ENUMERATION = 0x09;

		EnumerationValue() = default;
		EnumerationValue(const EnumerationSignature* signature, Value* value);
		~EnumerationValue();

		Type get_type() const override;
		Value* clone() const override;

		Enumeration to_enumeration() const override;

		static Value* read_enumeration(
			Type type, TraceFile& trace, FileStream& stream);

	private:
		Enumeration value;
	};

	class BitmaskValue : public Value
	{
	public:
		static const Type BITMASK = 0x0a;

		BitmaskValue() = default;
		BitmaskValue(const BitmaskSignature* signature, std::uint32_t value);

		Type get_type() const override;
		Value* clone() const override;

		Bitmask to_bitmask() const override;

		static Value* read_bitmask(
			Type type, TraceFile& trace, FileStream& stream);

	private:
		Bitmask value;
	};

	class ArrayValue : public Value
	{
	public:
		static const Type ARRAY = 0x0b;

		ArrayValue() = default;
		ArrayValue(const Array* array);

		Type get_type() const override;
		Value* clone() const override;

		const Array* to_array() const override;

		static Value* read_array(
			Type type, TraceFile& trace, FileStream& stream);
	private:
		Array value;
	};

	class StructureValue : public Value
	{
	public:
		static const Type STRUCTURE = 0x0c;

		StructureValue() = default;
		StructureValue(const StructureSignature* signature);

		Type get_type() const override;
		Value* clone() const override;

		const Structure* to_structure() const override;

		static Value* read_structure(
			Type type, TraceFile& trace, FileStream& stream);
		
	private:
		Structure* value;
	};

	class HandleValue : public Value
	{
	public:
		static const Type HANDLE = 0x0d;

		HandleValue() = default;
		explicit HandleValue(std::uint32_t value);

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
