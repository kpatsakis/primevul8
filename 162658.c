doinsert(p, op, opnd, pos)
register struct parse *p;
sop op;
size_t opnd;
sopno pos;
{
	register sopno sn;
	register sop s;
	register int i;

	/* avoid making error situations worse */
	if (p->error != 0)
		return;

	sn = HERE();
	EMIT(op, opnd);		/* do checks, ensure space */
	assert(HERE() == sn+1);
	s = p->strip[sn];

	/* adjust paren pointers */
	assert(pos > 0);
	for (i = 1; i < NPAREN; i++) {
		if (p->pbegin[i] >= pos) {
			p->pbegin[i]++;
		}
		if (p->pend[i] >= pos) {
			p->pend[i]++;
		}
	}
	{
          int length=(HERE()-pos-1)*sizeof(sop);
          bmove_upp((uchar *) &p->strip[pos+1]+length,
                    (uchar *) &p->strip[pos]+length,
                    length);
        }
#ifdef OLD_CODE
        memmove((char *)&p->strip[pos+1], (char *)&p->strip[pos],
                                                (HERE()-pos-1)*sizeof(sop));
#endif
	p->strip[pos] = s;
}