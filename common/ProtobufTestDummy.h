//---------------------------------------------------------------
//
// ProtobufTestDummy.h
//

#pragma once

#include "common/ProtoAdapter.h"
#include "proto/poc.pb.h"

#include <memory>
#include <string>

namespace Common {
namespace Test {

//===============================================================================

class ProtobufTestDummy : public ProtoAdapter<poc::ProtobufTestDummy, ProtobufTestDummy>
{
public:
	ProtobufTestDummy();
	ProtobufTestDummy(const Bytes& bytes);
	ProtobufTestDummy(int mana, int health, int speed, const std::string& name);
	~ProtobufTestDummy();

	// Adapter implementation.
	virtual ProtobufTestDummy FromBytes(const Bytes& bytes) const override;
	virtual const std::string& ToString() const override;
	virtual ProtobufTestDummy FromProtobuf(const poc::ProtobufTestDummy* creatureProto) const override;
	virtual poc::ProtobufTestDummy ToProtobuf() const override;

	int32_t GetMana() const { return m_mana; }
	int32_t GetHealth() const { return m_health; }
	int32_t GetSpeed() const { return m_speed; }
	const std::string& GetName() const { return m_name; }

private:
	void UpdateSerializedCache();

	int32_t m_mana;
	int32_t m_health;
	int32_t m_speed;
	std::string m_name;

	bool m_isCacheDirty = true;
	std::string m_serializedStringCache;
};

//===============================================================================

} // namespace Test
} // namespace Common
