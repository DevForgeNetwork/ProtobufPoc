//---------------------------------------------------------------
//
// MessageParserTest.h
//

#pragma once

#include <memory>

namespace Common {

class MessageParser;

namespace Test {

//===============================================================================

class MessageParserTest
{
public:
	MessageParserTest();
	~MessageParserTest();
	void RunTest();

private:
	std::unique_ptr<MessageParser> m_parser;
};

//===============================================================================

} // namespace Test
} // namespace Common
