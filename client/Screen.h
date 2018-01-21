//---------------------------------------------------------------
//
// Screen.h
//

#pragma once

namespace Client {

//===============================================================================

class Screen
{
public:
	// Event loop.
	virtual void ProcessEvents() = 0;

	// Render loop.
	virtual void Draw() = 0;
};

//===============================================================================

} // namespace Client
