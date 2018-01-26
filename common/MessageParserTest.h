//---------------------------------------------------------------
//
// MessageParserTest.h
//

#pragma once

#include <memory>
#include <vector>

namespace Common {

class MessageParser;
struct NetworkMessage;

namespace Test {

//===============================================================================
class MessageHelper;
class MessageParserTest
{
	friend class MessageHelper;
public:
	MessageParserTest();
	~MessageParserTest();
	void RunTests();

private:
	void ClearBuffer(uint8_t buffer[], int size);
	void Verify(const NetworkMessage& message);
	void TestSingle();
	void TestMulti();
	void TestPartial();
	void TestUneven();

	class MessageHelper
	{
	public:
		MessageHelper(MessageParserTest* tester);

		void SendPartialMessage(uint8_t data[], int sizeToSend,
			std::vector<NetworkMessage>& messages, bool& hasMessages);
		void SendPartialHeader(uint8_t data[], int sizeToSend,
			std::vector<NetworkMessage>& messages, bool& hasMessages);

		// This should be called between sending a header and a message.
		void Clear();

	private:
		void SendPartialData(uint8_t data[], int sizeToSend,
			std::vector<NetworkMessage>& messages, bool& hasMessages);

	private:
		MessageParserTest* m_tester;
		int m_sizeSent = 0;
	};

private:
	std::unique_ptr<MessageParser> m_parser;
	MessageHelper m_helper;
};

//===============================================================================

} // namespace Test
} // namespace Common
