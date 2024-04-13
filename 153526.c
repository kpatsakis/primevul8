setModtime(char *path, time_t modtime)
{
    struct utimbuf t;
    struct stat st;

    if (stat(path, &st) == 0)
	t.actime = st.st_atime;
    else
	t.actime = time(NULL);
    t.modtime = modtime;
    return utime(path, &t);
}