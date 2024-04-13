has_hidden_link(struct readbuffer *obuf, int cmd)
{
    Str line = obuf->line;
    struct link_stack *p;

    if (Strlastchar(line) != '>')
	return NULL;

    for (p = link_stack; p; p = p->next)
	if (p->cmd == cmd)
	    break;
    if (!p)
	return NULL;

    if (obuf->pos == p->pos)
	return line->ptr + p->offset;

    return NULL;
}