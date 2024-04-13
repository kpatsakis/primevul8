freeset(p, cs)
register struct parse *p;
register cset *cs;
{
	register size_t i;
	register cset *top = &p->g->sets[p->g->ncsets];
	register size_t css = (size_t)p->g->csetsize;

	for (i = 0; i < css; i++)
	  CHsub(cs, i);
	if (cs == top-1)	/* recover only the easy case */
	  p->g->ncsets--;
}