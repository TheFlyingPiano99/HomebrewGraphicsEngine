#include "DebugUtils.h"
#include <iostream>
#include <chrono>
#include "AssetFolderPathManager.h"


void Hogra::DebugUtils::PrintError(const char* context, const char* message)
{
	if (printPriority >= PrintPriority::pErrorOnly) {
		const auto now = std::chrono::system_clock::now();
		std::cerr << "ERROR: { Context: " << context << ", Time: "<< now << ", Msg: " << message << " }" << std::endl;
		if (logToFile) {
			
		}
	}
}

void Hogra::DebugUtils::PrintMsg(const char* context, const char* message)
{
	if (printPriority == PrintPriority::pAll) {
		const auto now = std::chrono::system_clock::now();
		std::cout << "Info: { Context: " << context << ", Time: " << now << ", Msg: " << message << " }" << std::endl;
		if (logToFile) {

		}
	}
}
