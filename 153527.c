check_command(char *cmd, int auxbin_p)
{
    static char *path = NULL;
    Str dirs;
    char *p, *np;
    Str pathname;
    struct stat st;

    if (path == NULL)
	path = getenv("PATH");
    if (auxbin_p)
	dirs = Strnew_charp(w3m_auxbin_dir());
    else
	dirs = Strnew_charp(path);
    for (p = dirs->ptr; p != NULL; p = np) {
	np = strchr(p, PATH_SEPARATOR);
	if (np)
	    *np++ = '\0';
	pathname = Strnew();
	Strcat_charp(pathname, p);
	Strcat_char(pathname, '/');
	Strcat_charp(pathname, cmd);
	if (stat(pathname->ptr, &st) == 0 && S_ISREG(st.st_mode)
	    && (st.st_mode & S_IXANY) != 0)
	    return 1;
    }
    return 0;
}