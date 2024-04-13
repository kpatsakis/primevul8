p_b_eclass(p, cs)
register struct parse *p;
register cset *cs;
{
	register char c;

	c = p_b_coll_elem(p, '=');
	CHadd(cs, c);
}