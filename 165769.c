int count_embedded_options(char *st)
{
  int rc;
  char c;
  char *ptr;

  ptr= st;
  rc= 0;

  if (st)
  {
    while ((c= *ptr++))
    {
      if (c == ',')
        rc++;
    }
    rc++;
  }

  return rc;
}