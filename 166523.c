_PUBLIC_ char *fgets_slash(char *s2,int maxlen,XFILE *f)
{
  char *s=s2;
  int len = 0;
  int c;
  bool start_of_line = true;

  if (x_feof(f))
    return(NULL);

  if (maxlen <2) return(NULL);

  if (!s2)
    {
      maxlen = MIN(maxlen,8);
      s = (char *)malloc(maxlen);
    }

  if (!s) return(NULL);

  *s = 0;

  while (len < maxlen-1)
    {
      c = x_getc(f);
      switch (c)
	{
	case '\r':
	  break;
	case '\n':
	  while (len > 0 && s[len-1] == ' ')
	    {
	      s[--len] = 0;
	    }
	  if (len > 0 && s[len-1] == '\\')
	    {
	      s[--len] = 0;
	      start_of_line = true;
	      break;
	    }
	  return(s);
	case EOF:
	  if (len <= 0 && !s2) 
	    SAFE_FREE(s);
	  return(len>0?s:NULL);
	case ' ':
	  if (start_of_line)
	    break;
	  /* fall through */
	default:
	  start_of_line = false;
	  s[len++] = c;
	  s[len] = 0;
	}
      if (!s2 && len > maxlen-3)
	{
	  char *t;
	  
	  maxlen *= 2;
	  t = realloc_p(s, char, maxlen);
	  if (!t) {
	    DEBUG(0,("fgets_slash: failed to expand buffer!\n"));
	    SAFE_FREE(s);
	    return(NULL);
	  } else s = t;
	}
    }
  return(s);
}