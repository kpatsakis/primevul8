num_append(char *s, int len, u_long x)
{
  char buf[30];
  char *t;

  if (!x)
    return str_append(s,len,"0");
  *(t = &buf[sizeof(buf)-1]) = '\0';
  while (x && (t > buf))
    {
      *--t = '0'+(x % 10);
      x /= 10;
    }
  return str_append(s,len,t);
}