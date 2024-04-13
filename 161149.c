get_tmfromtime(struct tm *tm, time_t *t)
{
#if HAVE_LOCALTIME_R
	tzset();
	localtime_r(t, tm);
#elif HAVE__LOCALTIME64_S
	_localtime64_s(tm, t);
#else
	memcpy(tm, localtime(t), sizeof(*tm));
#endif
}