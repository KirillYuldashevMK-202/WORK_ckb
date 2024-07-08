#include "MyPlugin.h"
#include "PluginInterface.h"

#include <iostream>
#include <fstream>
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

int main() {
#ifdef _WIN32
	loaderPlugins MainPlugin("C:/Users/Kirill/source/Work1/newLinux/Myplugin/");
#else
	loaderPlugins MainPlugin("/home/kirill/workckb/");
#endif
	MainPlugin.loadPlugin();
	MainPlugin.unloadPlugin();
	MainPlugin.executePlugin();
	return 0;
}
