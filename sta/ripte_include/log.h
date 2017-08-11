#define info(...) IMSG(__VA_ARGS__)
#ifdef RIPTE_DEBUG
	#define debug(...) DMSG(__VA_ARGS__)
#else
	#define debug(...) (void)0
#endif