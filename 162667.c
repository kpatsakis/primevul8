samesets(g, c1, c2)
register struct re_guts *g;
int c1;
int c2;
{
	register uch *col;
	register int i;
	register int ncols = (g->ncsets+(CHAR_BIT-1)) / CHAR_BIT;
	register unsigned uc1 = (unsigned char)c1;
	register unsigned uc2 = (unsigned char)c2;

	for (i = 0, col = g->setbits; i < ncols; i++, col += g->csetsize)
		if (col[uc1] != col[uc2])
			return(0);
	return(1);
}