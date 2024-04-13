ordinary(p, ch)
register struct parse *p;
register int ch;
{
	register cat_t *cap = p->g->categories;

	if ((p->g->cflags&REG_ICASE) && my_isalpha(p->charset,ch) && 
	     othercase(p->charset,ch) != ch)
		bothcases(p, ch);
	else {
		EMIT(OCHAR, (unsigned char)ch);
		if (cap[ch] == 0)
			cap[ch] = p->g->ncategories++;
	}
}