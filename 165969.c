append_snprintf (char * dest, size_t maxlen, const char * fmt, ...)
{	size_t len = strlen (dest) ;

	if (len < maxlen)
	{	va_list ap ;

		va_start (ap, fmt) ;
		vsnprintf (dest + len, maxlen - len, fmt, ap) ;
		va_end (ap) ;
		} ;

	return ;
} /* append_snprintf */