#include "MyPlugin.h"

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

#ifdef _WIN32

#else

#endif

int main() {

#ifdef _WIN32
	std::string pluginsDir = "C:/Users/Kirill/source/Work1/newLinux/Myplugin/";
	std::vector<std::string> pluginFiles;
	pluginFiles.push_back(pluginsDir + "nPluginMsgbox.dll");
	pluginFiles.push_back(pluginsDir + "nPluginTest.dll");
	HMODULE MyPlugins;
#else
	std::string pluginsDir = "/home/kirill/workckb/";
	std::vector<std::string> pluginFiles;
	pluginFiles.push_back(pluginsDir + "mymsg.so");
	pluginFiles.push_back(pluginsDir + "mytest.so");
	void* MyPlugins;
#endif

	for (int i = 0; i < pluginFiles.size();i+=1) {

		#ifdef _WIN32
		HMODULE MyPlugins = LoadLibrary(pluginFiles[i].c_str());
		#else
		MyPlugins = dlopen(pluginFiles[i].c_str(), RTLD_LAZY);
		#endif

		if (!MyPlugins) {

			std::cout << "Error: "<< pluginFiles[i]<<" not loaded" << std::endl;
			#ifdef _WIN32
			FreeLibrary(MyPlugins);
			#else
			dlclose(MyPlugins);
			#endif
			exit(1);

		}

		#ifdef _WIN32
		MYPROCcreate Mycreate = (MYPROCcreate)GetProcAddress(MyPlugins, "CreatePlugin");
		#else
		MYPROCcreate Mycreate = (MYPROCcreate)dlsym(MyPlugins, "CreatePlugin");
		#endif

		InterfacePlugin* Plugin = Mycreate();
		if (!Plugin) {
			std::cout << "Error: Plugin"<< pluginFiles[i] <<"not create" << std::endl;
			#ifdef _WIN32
			FreeLibrary(MyPlugins);
			#else
			dlclose(MyPlugins);
			#endif
			return 1;
		}

		Plugin->InitPlugin();
		Plugin->WorkPlugin();

		#ifdef _WIN32
		MYPROCdelete Mydelete = (MYPROCdelete)GetProcAddress(MyPlugins, "DeletePlugin");
		#else
		MYPROCdelete Mydelete = (MYPROCdelete)dlsym(MyPlugins, "DeletePlugin");
		#endif

		#ifdef _WIN32
		FreeLibrary(MyPlugins);
		#else
		dlclose(MyPlugins);
		#endif
	}
	return 0;
}
