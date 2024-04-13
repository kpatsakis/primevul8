stripsnug(p, g)
register struct parse *p;
register struct re_guts *g;
{
	g->nstates = p->slen;
	g->strip = (sop *)realloc((char *)p->strip, p->slen * sizeof(sop));
	if (g->strip == NULL) {
		SETERROR(REG_ESPACE);
		g->strip = p->strip;
	}
}