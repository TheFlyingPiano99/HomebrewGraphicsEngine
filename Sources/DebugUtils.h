#pragma once

namespace Hogra::DebugUtils {
	
	enum class PrintPriority {
		pNone,
		pErrorOnly,
		pAll
	};
	constexpr PrintPriority printPriority = PrintPriority::pAll;
	constexpr bool logToFile = false;

	void PrintError(const char* context = "Default", const char* message = "Error!");

	void PrintMsg(const char* context = "Default", const char* message = "unspecified");
}

