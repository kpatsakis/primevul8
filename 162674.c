firstch(p, cs)
register struct parse *p;
register cset *cs;
{
	register size_t i;
	register size_t css = (size_t)p->g->csetsize;

	for (i = 0; i < css; i++)
		if (CHIN(cs, i))
			return((char)i);
	assert(never);
	return(0);		/* arbitrary */
}