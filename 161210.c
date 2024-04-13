get_gmoffset(struct tm *tm)
{
	long offset;

#if defined(HAVE__GET_TIMEZONE)
	_get_timezone(&offset);
#elif defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
	offset = _timezone;
#else
	offset = timezone;
#endif
	offset *= -1;
	if (tm->tm_isdst)
		offset += 3600;
	return (offset);
}