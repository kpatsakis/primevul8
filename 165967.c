psf_get_date_str (char *str, int maxlen)
{	time_t		current ;
	struct tm	timedata, *tmptr ;

	time (&current) ;

#if defined (HAVE_GMTIME_R)
	/* If the re-entrant version is available, use it. */
	tmptr = gmtime_r (&current, &timedata) ;
#elif defined (HAVE_GMTIME)
	/* Otherwise use the standard one and copy the data to local storage. */
	tmptr = gmtime (&current) ;
	memcpy (&timedata, tmptr, sizeof (timedata)) ;
#else
	tmptr = NULL ;
#endif

	if (tmptr)
		snprintf (str, maxlen, "%4d-%02d-%02d %02d:%02d:%02d UTC",
			1900 + timedata.tm_year, timedata.tm_mon, timedata.tm_mday,
			timedata.tm_hour, timedata.tm_min, timedata.tm_sec) ;
	else
		snprintf (str, maxlen, "Unknown date") ;

	return ;
} /* psf_get_date_str */