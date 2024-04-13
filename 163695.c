struct chunk *http_error_message(struct session *s, int msgnum)
{
	if (s->be->errmsg[msgnum].str)
		return &s->be->errmsg[msgnum];
	else if (s->fe->errmsg[msgnum].str)
		return &s->fe->errmsg[msgnum];
	else
		return &http_err_chunks[msgnum];
}