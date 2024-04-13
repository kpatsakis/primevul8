static char * check_for_domain(char **ppuser)
{
	char * original_string;
	char * usernm;
	char * domainnm;
	int    original_len;
	int    len;
	int    i;

	if(ppuser == NULL)
		return NULL;

	original_string = *ppuser;

	if (original_string == NULL)
		return NULL;
	
	original_len = strlen(original_string);

	usernm = strchr(*ppuser,'/');
	if (usernm == NULL) {
		usernm = strchr(*ppuser,'\\');
		if (usernm == NULL)
			return NULL;
	}

	if(got_domain) {
		fprintf(stderr, "Domain name specified twice. Username probably malformed\n");
		return NULL;
	}

	usernm[0] = 0;
	domainnm = *ppuser;
	if (domainnm[0] != 0) {
		got_domain = 1;
	} else {
		fprintf(stderr, "null domain\n");
	}
	len = strlen(domainnm);
	/* reset domainm to new buffer, and copy
	domain name into it */
	domainnm = (char *)malloc(len+1);
	if(domainnm == NULL)
		return NULL;

	strlcpy(domainnm,*ppuser,len+1);

/*	move_string(*ppuser, usernm+1) */
	len = strlen(usernm+1);

	if(len >= original_len) {
		/* should not happen */
		return domainnm;
	}

	for(i=0;i<original_len;i++) {
		if(i<len)
			original_string[i] = usernm[i+1];
		else /* stuff with commas to remove last parm */
			original_string[i] = ',';
	}

	/* BB add check for more than one slash? 
	  strchr(*ppuser,'/');
	  strchr(*ppuser,'\\') 
	*/
	
	return domainnm;
}