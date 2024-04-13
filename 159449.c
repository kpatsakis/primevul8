nv_compare(const void *s1, const void *s2)
{
    int		c1, c2;

    // The commands are sorted on absolute value.
    c1 = nv_cmds[*(const short *)s1].cmd_char;
    c2 = nv_cmds[*(const short *)s2].cmd_char;
    if (c1 < 0)
	c1 = -c1;
    if (c2 < 0)
	c2 = -c2;
    return c1 - c2;
}