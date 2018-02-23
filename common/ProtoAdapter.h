//---------------------------------------------------------------
//
// ProtoAdapter.h
//

#pragma once

#include <utility>
#include <cstdint>
#include <string>

namespace Common {

//===============================================================================

using Bytes = std::pair<const char*, uint32_t>;

// Convert back and forth between proto and class representation of it.
// proto, class representation.
template<class T, class U>
class ProtoAdapter
{
public:

	// Construct the class object from a stream of raw bytes representing the proto.
	virtual U FromBytes(const Bytes& bytes) const = 0;

	// Return raw bytes from the protobuf.
	virtual const std::string& ToString() const = 0;

	// Returns our class representation of the protobuf. Usually just a collection of data.
	virtual U FromProtobuf(const T* proto) const = 0;

	// Returns the protobuf representation of this class.
	virtual T ToProtobuf() const = 0;
};

//===============================================================================
} // namespace Common
