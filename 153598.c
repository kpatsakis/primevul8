dir_exist(char *path)
{
    struct stat stbuf;

    if (path == NULL || *path == '\0')
	return 0;
    if (stat(path, &stbuf) == -1)
	return 0;
    return IS_DIRECTORY(stbuf.st_mode);
}