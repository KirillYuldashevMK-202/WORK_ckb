#ifndef MY_PLUGIN_H
#define MY_PLUGIN_H

class InterfacePlugin {
public:
    virtual ~InterfacePlugin() {}
    virtual void InitPlugin() = 0;
    virtual void WorkPlugin() = 0;
};

extern "C" {
    InterfacePlugin* CreatePlugin();
    void DeletePlugin(InterfacePlugin* plugin);
}

#endif

