js_fmtexp(char *p, int e)
{
	char se[9];
	int i;

	*p++ = 'e';
	if(e < 0) {
		*p++ = '-';
		e = -e;
	} else
		*p++ = '+';
	i = 0;
	while(e) {
		se[i++] = e % 10 + '0';
		e /= 10;
	}
	while(i < 1)
		se[i++] = '0';
	while(i > 0)
		*p++ = se[--i];
	*p++ = '\0';
}