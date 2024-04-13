p_b_coll_elem(p, endc)
register struct parse *p;
int endc;			/* name ended by endc,']' */
{
	register char *sp = p->next;
	register struct cname *cp;
#ifdef _WIN64
	register __int64 len;
#else
	register int len;
#endif
	while (MORE() && !SEETWO(endc, ']'))
		NEXT();
	if (!MORE()) {
		SETERROR(REG_EBRACK);
		return(0);
	}
	len = p->next - sp;
	for (cp = cnames; cp->name != NULL; cp++)
		if (strncmp(cp->name, sp, len) == 0 && cp->name[len] == '\0')
			return(cp->code);	/* known name */
	if (len == 1)
		return(*sp);	/* single character */
	SETERROR(REG_ECOLLATE);			/* neither */
	return(0);
}