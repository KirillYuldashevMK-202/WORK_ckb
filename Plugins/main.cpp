#include "MyPlugin.h"
#include <iostream>
#include <Windows.h>

int main() {
	HMODULE MyPlugins = LoadLibraryA("PluginMsgbox.dll");
	if (!MyPlugins) {
		std::cout << "Error: Plugin not loaded" << std::endl;
		exit(1);
	}

	InterfacePlugin* MsgPlugin = CreatePlugin();
	if (!MsgPlugin) {
		std::cout << "Error: Plugin not create" << std::endl;
		FreeLibrary(MyPlugins);
		return 1;
	}

	MsgPlugin->InitPlugin();
	MsgPlugin->WorkPlugin();

	return 0;
}
