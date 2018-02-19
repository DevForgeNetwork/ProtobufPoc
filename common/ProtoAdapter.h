//---------------------------------------------------------------
//
// ProtoAdapter.h
//

#pragma once

#include <utility>
#include <cstdint>

namespace Common {

//===============================================================================

// Convert back and forth between proto and class representation of it.
// proto, class representation.
template<class T, class U>
class ProtoAdapter
{
public:

	// Return raw bytes from the protobuf.
	virtual std::pair<uint8_t*, int> ToBytes() const = 0;

	// Returns our class representation of the protobuf. Usually just a collection of data.
	virtual U FromProtobuf(const T* proto) const = 0;

	// Returns the protobuf representation of this class.
	virtual T ToProtobuf() const = 0;
};

//===============================================================================
} // namespace Common
