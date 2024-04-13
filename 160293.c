void timer_start(starttime_t& r)
{
#ifdef LIBRAW_WIN32_CALLS
	QueryPerformanceCounter(&r.started);
#else
	gettimeofday(&r.started,NULL);
#endif
}