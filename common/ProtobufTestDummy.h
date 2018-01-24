//---------------------------------------------------------------
//
// ProtobufTestDummy.h
//

#pragma once

#include "proto/poc.pb.h"

#include <memory>
#include <string>

namespace Common {
namespace Test {

//===============================================================================

class ProtobufTestDummy
{
public:
	ProtobufTestDummy();
	ProtobufTestDummy(int mana, int health, int speed, const std::string& name);
	~ProtobufTestDummy();

	// Returns the array of serialized data in bytes.
	std::pair<uint8_t*, int> ToBytes();
	
	// Convert to struct.
	ProtobufTestDummy FromProtobuf(const poc::ProtobufTestDummy& creatureProto);

	// Convert to protobuf.
	poc::ProtobufTestDummy ToProtobuf();

private:
	int32_t m_mana;
	int32_t m_health;
	int32_t m_speed;
	std::string m_name;
};

//===============================================================================

} // namespace Test
} // namespace Common
