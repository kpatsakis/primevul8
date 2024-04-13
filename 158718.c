pdf_nextlinestart(const char *ptr, size_t len)
{
	while(strchr("\r\n", *ptr) == NULL) {
		if(--len == 0L)
			return NULL;
		ptr++;
	}
	while(strchr("\r\n", *ptr) != NULL) {
		if(--len == 0L)
			return NULL;
		ptr++;
	}
	return ptr;
}