bothcases(p, ch)
register struct parse *p;
int ch;
{
	register char *oldnext = p->next;
	register char *oldend = p->end;
	char bracket[3];

	assert(othercase(p->charset, ch) != ch); /* p_bracket() would recurse */
	p->next = bracket;
	p->end = bracket+2;
	bracket[0] = ch;
	bracket[1] = ']';
	bracket[2] = '\0';
	p_bracket(p);
	assert(p->next == bracket+2);
	p->next = oldnext;
	p->end = oldend;
}