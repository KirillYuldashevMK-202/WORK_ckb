#include "MyPlugin.h"
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <vector>

typedef InterfacePlugin* (*CreatePluginFunc)();
typedef void (*DeletePluginFunc)(InterfacePlugin*);

int main() {
	std::string pluginsDirectory = "Myplugin/";
	std::vector<std::string> pluginFiles;
	pluginFiles.push_back(pluginsDirectory + "PluginTest.dll");
	pluginFiles.push_back(pluginsDirectory + "PluginMsgbox.dll");
	for (int i = 0; i < pluginFiles.size();i+=1) {
		HMODULE MyPlugins = LoadLibraryA(pluginFiles[i].c_str());
		if (!MyPlugins) {
			std::cerr << "Error DLL: " << pluginFiles[i] << std::endl;
			continue;
		}
		CreatePluginFunc CreatePlugin = nullptr;
		if (pluginFiles[i] == pluginsDirectory + "PluginMsgbox.dll") {
			CreatePluginFunc CreatePlugin = (CreatePluginFunc)GetProcAddress(MyPlugins, "CreatePluginMsgbox");
		}
		else if (pluginFiles[i] == pluginsDirectory + "PluginTest.dll") {
			CreatePluginFunc CreatePlugin = (CreatePluginFunc)GetProcAddress(MyPlugins, "CreatePluginTest");
		}
		InterfacePlugin* Plugin = CreatePlugin();
		if (!Plugin) {
			std::cout << "Error: Plugin " << pluginFiles[i] << " not create" << std::endl;
			continue;
		}
		Plugin->InitPlugin();
		Plugin->WorkPlugin();

		DeletePluginFunc DeletePlugin = nullptr;
		if (pluginFiles[i] == "PluginMsgbox.dll") {
			DeletePluginFunc DeletePlugin = (DeletePluginFunc)GetProcAddress(MyPlugins, "DeletePluginMsgbox");
		}
		else if (pluginFiles[i] == "PluginTest.dll") {
			DeletePluginFunc DeletePlugin = (DeletePluginFunc)GetProcAddress(MyPlugins, "DeletePluginTest");
		}
		DeletePlugin(Plugin);
	}

	
	return 0;
}
