log_create(uschar *name)
{
int fd = Uopen(name,
#ifdef O_CLOEXEC
	O_CLOEXEC |
#endif
	O_CREAT|O_APPEND|O_WRONLY, LOG_MODE);

/* If creation failed, attempt to build a log directory in case that is the
problem. */

if (fd < 0 && errno == ENOENT)
  {
  BOOL created;
  uschar *lastslash = Ustrrchr(name, '/');
  *lastslash = 0;
  created = directory_make(NULL, name, LOG_DIRECTORY_MODE, FALSE);
  DEBUG(D_any) debug_printf("%s log directory %s\n",
    created ? "created" : "failed to create", name);
  *lastslash = '/';
  if (created) fd = Uopen(name,
#ifdef O_CLOEXEC
			O_CLOEXEC |
#endif
		       	O_CREAT|O_APPEND|O_WRONLY, LOG_MODE);
  }

return fd;
}