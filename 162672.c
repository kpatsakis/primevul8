pluscount(p, g)
struct parse *p;
register struct re_guts *g;
{
	register sop *scan;
	register sop s;
	register sopno plusnest = 0;
	register sopno maxnest = 0;

	if (p->error != 0)
		return(0);	/* there may not be an OEND */

	scan = g->strip + 1;
	do {
		s = *scan++;
		switch (OP(s)) {
		case OPLUS_:
			plusnest++;
			break;
		case O_PLUS:
			if (plusnest > maxnest)
				maxnest = plusnest;
			plusnest--;
			break;
		}
	} while (OP(s) != OEND);
	if (plusnest != 0)
		g->iflags |= BAD;
	return(maxnest);
}