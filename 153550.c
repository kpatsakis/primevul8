push_render_image(Str str, int width, int limit,
		  struct html_feed_environ *h_env)
{
    struct readbuffer *obuf = h_env->obuf;
    int indent = h_env->envs[h_env->envc].indent;

    push_spaces(obuf, 1, (limit - width) / 2);
    push_str(obuf, width, str, PC_ASCII);
    push_spaces(obuf, 1, (limit - width + 1) / 2);
    if (width > 0)
	flushline(h_env, obuf, indent, 0, h_env->limit);
}