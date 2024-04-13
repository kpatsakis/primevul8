static size_t StringSignature(const char* string)
{
  size_t
    n,
    i,
    j,
    signature,
    stringLength;

  union
  {
    const char* s;
    const size_t* u;
  } p;

  stringLength=(size_t) strlen(string);
  signature=stringLength;
  n=stringLength/sizeof(size_t);
  p.s=string;
  for (i = 0; i < n; i++)
    signature^=p.u[i];
  if (n * sizeof(size_t) != stringLength)
    {
      char
        padded[4];

      j=n*sizeof(size_t);
      for (i = 0; i < 4; i++, j++)
      {
        if (j < stringLength)
          padded[i]=p.s[j];
        else
          padded[i]=0;
      }
      p.s=padded;
      signature^=p.u[0];
    }
  return(signature);
}