//---------------------------------------------------------------
//
// ProtobufTestDummy.h
//

#include "ProtobufTestDummy.h"

namespace Common {
namespace Test {

//===============================================================================

ProtobufTestDummy::ProtobufTestDummy()
{
}

ProtobufTestDummy::ProtobufTestDummy(int mana, int health, int speed, const std::string& name)
	: m_mana(mana)
	, m_health(health)
	, m_speed(speed)
	, m_name(name)
{
}

ProtobufTestDummy::~ProtobufTestDummy()
{

}

std::pair<uint8_t*, int> ProtobufTestDummy::ToBytes()
{
	poc::ProtobufTestDummy testProto = ToProtobuf();

	// Serialize the protobuf message to string.
	std::string serializedProto = testProto.SerializeAsString();

	auto buffer = std::make_unique<uint8_t[]>(serializedProto.length());
	std::memcpy(buffer.get(), &serializedProto, serializedProto.length());

	return { buffer.get(), testProto.ByteSize() };
}

ProtobufTestDummy ProtobufTestDummy::FromProtobuf(const poc::ProtobufTestDummy& testProto)
{
	return ProtobufTestDummy{ testProto.mana(), testProto.health(),
		testProto.speed(), testProto.name() };
}

poc::ProtobufTestDummy ProtobufTestDummy::ToProtobuf()
{
	poc::ProtobufTestDummy protobufTestDummy;
	protobufTestDummy.set_mana(m_mana);
	protobufTestDummy.set_health(m_health);
	protobufTestDummy.set_speed(m_speed);
	protobufTestDummy.set_name(m_name);

	return protobufTestDummy;
}

//===============================================================================

} // namespace Test
} // namespace Common
