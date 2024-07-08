#include "MyPlugin.h"
#include "PluginInterface.h"

#include <iostream>
#include <fstream>
#include <vector>

int main() {
	//loaderPlugins MainPlugin("C:/Users/Kirill/source/Work1/newLinux/Myplugin/");
	loaderPlugins MainPlugin("/home/kirill/workckb/");
	MainPlugin.loadPlugin();
	MainPlugin.unloadPlugin();
	MainPlugin.executePlugin();
	return 0;
}
