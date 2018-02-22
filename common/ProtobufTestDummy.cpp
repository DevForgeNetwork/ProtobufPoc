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
	UpdateSerializedCache();
}

ProtobufTestDummy::ProtobufTestDummy(const Bytes& bytes)
{
	poc::ProtobufTestDummy proto = ToProtobuf();
	proto.ParseFromArray(bytes.first, bytes.second);

	m_health = proto.health();
	m_mana = proto.mana();
	m_speed = proto.speed();
	m_name = proto.name();

	UpdateSerializedCache();
}

ProtobufTestDummy::~ProtobufTestDummy()
{
}

ProtobufTestDummy ProtobufTestDummy::FromBytes(const Bytes& bytes) const
{
	poc::ProtobufTestDummy proto;
	proto.ParseFromArray(bytes.first, bytes.second);
	return ProtobufTestDummy{ proto.mana(), proto.health(), proto.speed(), proto.name() };
}

const std::string& ProtobufTestDummy::ToString() const
{
	return m_serializedStringCache;
}

ProtobufTestDummy ProtobufTestDummy::FromProtobuf(const poc::ProtobufTestDummy* creatureProto) const
{
	return ProtobufTestDummy{ creatureProto->mana(), creatureProto->health(),
		creatureProto->speed(), creatureProto->name() };
}

poc::ProtobufTestDummy ProtobufTestDummy::ToProtobuf() const
{
	poc::ProtobufTestDummy protobufTestDummy;
	protobufTestDummy.set_mana(m_mana);
	protobufTestDummy.set_health(m_health);
	protobufTestDummy.set_speed(m_speed);
	protobufTestDummy.set_name(m_name);

	return protobufTestDummy;
}

void ProtobufTestDummy::UpdateSerializedCache()
{
	if (m_isCacheDirty)
	{
		m_isCacheDirty = false;

		// Serialize the protobuf message to string.
		m_serializedStringCache = ToProtobuf().SerializeAsString();
	}
}

//===============================================================================

} // namespace Test
} // namespace Common
