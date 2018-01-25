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
	void Verify(const NetworkMessage& message) const;

private:
	std::unique_ptr<MessageParser> m_parser;
};

//===============================================================================

} // namespace Test
} // namespace Common
