static int compat_mkdir(const char *path, int mode)
{
	return mkdir(path);
}