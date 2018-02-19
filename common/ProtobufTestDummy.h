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
	ProtobufTestDummy(int mana, int health, int speed, const std::string& name);
	~ProtobufTestDummy();

	// Adapter implementation.
	virtual std::pair<uint8_t*, int> ToBytes() const override;
	virtual ProtobufTestDummy FromProtobuf(const poc::ProtobufTestDummy* creatureProto) const override;
	virtual poc::ProtobufTestDummy ToProtobuf() const override;

private:
	int32_t m_mana;
	int32_t m_health;
	int32_t m_speed;
	std::string m_name;
};

//===============================================================================

} // namespace Test
} // namespace Common
