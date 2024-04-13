make_caption(struct table *t, struct html_feed_environ *h_env)
{
    struct html_feed_environ henv;
    struct readbuffer obuf;
    struct environment envs[MAX_ENV_LEVEL];
    int limit;

    if (t->caption->length <= 0)
	return;

    if (t->total_width > 0)
	limit = t->total_width;
    else
	limit = h_env->limit;
    init_henv(&henv, &obuf, envs, MAX_ENV_LEVEL, newTextLineList(),
	      limit, h_env->envs[h_env->envc].indent);
    HTMLlineproc1("<center>", &henv);
    HTMLlineproc0(t->caption->ptr, &henv, FALSE);
    HTMLlineproc1("</center>", &henv);

    if (t->total_width < henv.maxlimit)
	t->total_width = henv.maxlimit;
    limit = h_env->limit;
    h_env->limit = t->total_width;
    HTMLlineproc1("<center>", h_env);
    HTMLlineproc0(t->caption->ptr, h_env, FALSE);
    HTMLlineproc1("</center>", h_env);
    h_env->limit = limit;
}