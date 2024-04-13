double timer_elapsed(const starttime_t& start)
{
#ifdef LIBRAW_WIN32_CALLS
	LARGE_INTEGER ended, freq;
	QueryPerformanceCounter(&ended);
	QueryPerformanceFrequency(&freq);
	return double(ended.QuadPart - start.started.QuadPart) / double(freq.QuadPart);
#else
	struct timeval ended;
	gettimeofday(&ended, NULL);
	return double(ended.tv_sec - start.started.tv_sec) + double(ended.tv_usec - start.started.tv_usec) / 1000000.0;
#endif
}