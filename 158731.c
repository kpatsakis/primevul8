pdf_nextobject(const char *ptr, size_t len)
{
	const char *p;
	int inobject = 1;

	while(len) {
		switch(*ptr) {
			case '\n':
			case '\r':
			case '%':	/* comment */
				p = pdf_nextlinestart(ptr, len);
				if(p == NULL)
					return NULL;
				len -= (size_t)(p - ptr);
				ptr = p;
				inobject = 0;
				break;

			case ' ':
			case '\t':
			case '[':	/* Start of an array object */
			case '\v':
			case '\f':
			case '<':	/* Start of a dictionary object */
				inobject = 0;
				ptr++;
				len--;
				break;
			case '/':	/* Start of a name object */
				return ptr;
			default:
				if(!inobject)
					/* TODO: parse and return object type */
					return ptr;
				ptr++;
				len--;
		}
	}
	return NULL;
}