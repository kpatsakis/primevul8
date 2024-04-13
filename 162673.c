p_b_cclass(p, cs)
register struct parse *p;
register cset *cs;
{
	register char *sp = p->next;
	register struct cclass *cp;
	register size_t len;
	
	while (MORE() && my_isalpha(p->charset,PEEK()))
		NEXT();
	len = p->next - sp;
	for (cp = cclasses; cp->name != NULL; cp++)
		if (strncmp(cp->name, sp, len) == 0 && cp->name[len] == '\0')
			break;
	if (cp->name == NULL) {
		/* oops, didn't find it */
		SETERROR(REG_ECTYPE);
		return;
	}

#ifndef USE_ORIG_REGEX_CODE
	{
		register size_t i;
		for (i=1 ; i<256 ; i++)
			if (p->charset->ctype[i+1] & cp->mask)
				CHadd(cs, i);
	}
#else	
	{
		register char *u = (char*) cp->chars;
		register char c;
		
		while ((c = *u++) != '\0')
			CHadd(cs, c);
		
		for (u = (char*) cp->multis; *u != '\0'; u += strlen(u) + 1)
			MCadd(p, cs, u);
	}
#endif

}