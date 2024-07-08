#include <iostream>
#include <vector>
#include "MyPlugin.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

class loaderPlugins {
public:
	loaderPlugins(std::string PathPlugins) {
		this->PluginPath = PathPlugins;
	}
	void loadPlugin();
	void unloadPlugin();
	void executePlugin();
	~loaderPlugins() {
		for (int i = 0; i < this->Plugin.size(); i += 1) {
#ifdef _WIN32
			FreeLibrary(this->MyPlugins[i]);
#else
			dlclose(this->MyPlugins[i]);
#endif
		}
	}

private:
	std::string PluginPath;
	std::vector <InterfacePlugin*> Plugin;
#ifdef _WIN32
	std::vector <HMODULE> MyPlugins;
#else
	std::vector <void*> MyPlugins;
#endif
};
