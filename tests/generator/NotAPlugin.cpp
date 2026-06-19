#if defined(_WIN32)
    #define NOT_A_PLUGIN_EXPORT __declspec(dllexport)
#else
    #define NOT_A_PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

extern "C" NOT_A_PLUGIN_EXPORT int NotAPluginMarker()
{
    return 0;
}
