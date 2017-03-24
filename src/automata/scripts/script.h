#pragma once
#include <lua.hpp>

// A base script class, used as a parent to all script classes
// It is also used when no script is loaded

class Script {
	public:
		virtual void Step() { };
		virtual void LoadScript() { };
};