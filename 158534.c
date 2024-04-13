modefopen(const uschar *filename, const char *options, mode_t mode)
{
mode_t saved_umask = umask(0777);
FILE *f = Ufopen(filename, options);
(void)umask(saved_umask);
if (f != NULL) (void)fchmod(fileno(f), mode);
return f;
}