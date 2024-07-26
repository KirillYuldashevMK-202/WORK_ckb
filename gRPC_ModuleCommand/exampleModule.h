#ifndef FILES_MODULE
#define FILES_MODULE

#include "InterfaceModule.h"
#include <filesystem>
#include <thread>
#include <string>

class Files : public Module1 {
public:
	std::string Command(const std::string& command) override {
		if (command == "Files") {
			std::string namesFiles;
			std::string path = "C:/Users/Kirill";
			for (const auto& entry : std::filesystem::directory_iterator(path)) {
				namesFiles += entry.path().string().erase(0, 15) + "\n";
			}
			return namesFiles;
		}
		else {
			return "Unknown";
		}
	}
};

#endif
