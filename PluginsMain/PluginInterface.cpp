#include "PluginInterface.h"
#include "MyPlugin.h"
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#ifdef _WIN32
typedef InterfacePlugin* (__cdecl* MYPROCcreate)();
typedef void(__cdecl* MYPROCdelete)(InterfacePlugin*);
#else
typedef InterfacePlugin* (*MYPROCcreate)();
typedef void* (*MYPROCdelete)(InterfacePlugin*);
#endif

void loaderPlugins::loadPlugin() {
	std::vector<std::string> pluginFiles;
	std::string pluginsDir = this->PluginPath;
#ifdef _WIN32
	pluginFiles.push_back(pluginsDir + "nPluginMsgbox.dll");
	pluginFiles.push_back(pluginsDir + "nPluginTest.dll");
#else
	pluginFiles.push_back(pluginsDir + "mymsg.so");
	pluginFiles.push_back(pluginsDir + "mytest.so");
#endif

	for (int i = 0; i < pluginFiles.size(); i += 1) {
#ifdef _WIN32
		this->MyPlugins.push_back(LoadLibrary(pluginFiles[i].c_str()));
		if (!this->MyPlugins[i]) {
			FreeLibrary(MyPlugins[i]);
			std::cout << "Error: " << pluginFiles[i] << " not loaded" << std::endl;
			exit(1);
		}
#else
		this->MyPlugins.push_back(dlopen(pluginFiles[i].c_str(), RTLD_LAZY));
		if (!this->MyPlugins[i]) {
			dlclose(MyPlugins[i]);
			std::cout << "Error: " << pluginFiles[i] << " not loaded" << std::endl;
			exit(1);
		}
#endif
	}
}

void loaderPlugins::unloadPlugin() {
	std::vector <MYPROCcreate> Mycreate;
	for (int i = 0; i < this->MyPlugins.size(); i += 1) {
#ifdef _WIN32
		MYPROCcreate Mycreate = (MYPROCcreate)GetProcAddress(this->MyPlugins[i], "CreatePlugin");
		this->Plugin.push_back(Mycreate());
		if (!this->Plugin[i]) {
			std::cout << "Error: Plugin" << this->MyPlugins[i] << "not create" << std::endl;
			FreeLibrary(MyPlugins[i]);
			exit(1);
		}
#else
		MYPROCcreate Mycreate = (MYPROCcreate)dlsym(this->MyPlugins[i], "CreatePlugin");
		this->Plugin.push_back(Mycreate());
		if (!this->Plugin[i]) {
			std::cout << "Error: Plugin" << this->MyPlugins[i] << "not create" << std::endl;
			dlclose(MyPlugins[i]);
			exit(1);
		}
#endif
	}
}

void loaderPlugins::executePlugin() {
	for (int i = 0; i < this->Plugin.size(); i += 1) {
		this->Plugin[i]->InitPlugin();
		this->Plugin[i]->WorkPlugin();
	}
}




