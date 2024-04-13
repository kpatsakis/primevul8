hex_append(char *s, int len, u_long x)
{
  char buf[30];
  char *t;

  if (!x)
    return str_append(s,len,"0");
  *(t = &buf[sizeof(buf)-1]) = '\0';
  while (x && (t > buf))
    {
      u_int cc = (x % 16);
      *--t = ((cc < 10) ? ('0'+cc) : ('a'+cc-10));
      x /= 16;
    }
  return str_append(s,len,t);
}