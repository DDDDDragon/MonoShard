#ifdef _WIN64
	#define GMS2EXPORT extern "C" __declspec(dllexport)
#else
	#define GMS2EXPORT extern "C"
#endif
