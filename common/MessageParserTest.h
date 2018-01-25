//---------------------------------------------------------------
//
// MessageParserTest.h
//

#pragma once

#include <memory>

namespace Common {

class MessageParser;
struct NetworkMessage;

namespace Test {

//===============================================================================

class MessageParserTest
{
public:
	MessageParserTest();
	~MessageParserTest();
	void RunTests();

private:
	void ClearBuffer(uint8_t buffer[]) const;
	void Verify(const NetworkMessage& message) const;
	void TestSingle() const;
	void TestMulti() const;
	void TestPartial() const;
	void TestUneven() const;

private:
	std::unique_ptr<MessageParser> m_parser;
};

//===============================================================================

} // namespace Test
} // namespace Common
