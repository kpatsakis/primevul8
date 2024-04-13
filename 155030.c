cmd_with_count(
    char	*cmd,
    char_u	*bufp,
    size_t	bufsize,
    long	Prenum)
{
    size_t	len = STRLEN(cmd);

    STRCPY(bufp, cmd);
    if (Prenum > 0)
	vim_snprintf((char *)bufp + len, bufsize - len, "%ld", Prenum);
}