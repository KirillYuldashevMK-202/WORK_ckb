#include <iostream>
#include "MyPlugin.h"
#include <Windows.h>

class PluginMsgbox : public InterfacePlugin {
	void InitPlugin() override {
		std::cout << "PluginMsgbox init" << std::endl;
	}
	void WorkPlugin() override {
		MessageBoxA(NULL, "Hello world!", "STR", MB_OK);
	}
};

extern "C" InterfacePlugin* CreatePlugin() {
	return new PluginMsgbox();
}

extern "C" void DeletePlugin(InterfacePlugin* plugin) {
	delete plugin;
}
