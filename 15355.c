safe_strerror(int errnum)
{
  const char *s = strerror(errnum);
  return (s != NULL) ? s : "Unknown error";
}