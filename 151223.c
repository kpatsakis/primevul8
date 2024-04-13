static unsigned char *SFWScan(const unsigned char *p,const unsigned char *q,
  const unsigned char *target,const size_t length)
{
  register ssize_t
    i;

  while ((p+length) < q)
  {
    for (i=0; i < (ssize_t) length; i++)
      if (p[i] != target[i])
        break;
    if (i == (ssize_t) length)
      return((unsigned char *) p);
    p++;
  }
  return((unsigned char *) NULL);
}