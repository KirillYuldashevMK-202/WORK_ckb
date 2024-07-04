#include <iostream>
#include "MyPlugin.h"
#include <Windows.h>

class PluginTest : public InterfacePlugin {
	void InitPlugin() override {
		std::cout << "PluginTest init" << std::endl;
	}
	void WorkPlugin() override {
		std::cout << "PluginTest work" << std::endl;
	}
};

extern "C" InterfacePlugin* CreatePlugin() {
	return new PluginTest();
}

extern "C" void DeletePlugin(InterfacePlugin* plugin) {
	delete plugin;
}
