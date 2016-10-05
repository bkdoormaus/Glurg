// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_SIGnATURE_REGISTRY_HPP
#define LIBGLURG_TRACE_SIGnATURE_REGISTRY_HPP

#include <unordered_map>
#include <memory>

namespace glurg
{
	template <typename ID, typename Signature>
	class SignatureRegistry
	{
	public:
		SignatureRegistry() = default;
		~SignatureRegistry() = default;

		bool has_signature(ID id) const;
		const Signature* get_signature(ID id) const;

		void register_signature(ID id, Signature* signature);

	private:
		typedef std::unique_ptr<Signature> SignaturePointer;
		std::unordered_map<ID, SignaturePointer> signatures;
	};
}

template <typename ID, typename Signature>
bool glurg::SignatureRegistry<ID, Signature>::has_signature(ID id) const
{
	return this->signatures.find(id) != this->signatures.end();
}

template <typename ID, typename Signature>
const Signature* glurg::SignatureRegistry<ID, Signature>::get_signature(
	ID id) const
{
	auto e = this->signatures.find(id);
	if (e == this->signatures.end())
	{
		return nullptr;
	}

	return e->second.get();
}

template <typename ID, typename Signature>
void glurg::SignatureRegistry<ID, Signature>::register_signature(
	ID id, Signature* signature)
{
	this->signatures.insert(std::make_pair((id), SignaturePointer(signature)));
}

#endif
