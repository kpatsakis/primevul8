nch(p, cs)
register struct parse *p;
register cset *cs;
{
	register size_t i;
	register size_t css = (size_t)p->g->csetsize;
	register int n = 0;

	for (i = 0; i < css; i++)
		if (CHIN(cs, i))
			n++;
	return(n);
}