#include "MyPlugin.h"
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <vector>

typedef InterfacePlugin* (__cdecl* MYPROCcreate)();
typedef void(__cdecl* MYPROCdelete)(InterfacePlugin*);

int main() {
	std::string pluginsDir = "Myplugin/";
	std::vector<std::string> pluginFiles;
	pluginFiles.push_back(pluginsDir + "PluginTest.dll");
	pluginFiles.push_back(pluginsDir + "PluginMsgbox.dll");

	for (int i = 0; i < pluginFiles.size();i+=1) {
		HMODULE MyPlugins = LoadLibraryA(pluginFiles[i].c_str());
		if (!MyPlugins) {
			std::cout << "Error: "<< pluginFiles[i]<<" not loaded" << std::endl;
			exit(1);
		}
		MYPROCcreate Mycreate = (MYPROCcreate)GetProcAddress(MyPlugins, "CreatePlugin");
		InterfacePlugin* Plugin = Mycreate();
		if (!Plugin) {
			std::cout << "Error: Plugin"<< pluginFiles[i] <<"not create" << std::endl;
			FreeLibrary(MyPlugins);
			return 1;
		}
		Plugin->InitPlugin();
		Plugin->WorkPlugin();

		MYPROCdelete Mydelete = (MYPROCdelete)GetProcAddress(MyPlugins, "DeletePlugin");
		Mydelete(Plugin);
	}
	return 0;
}
