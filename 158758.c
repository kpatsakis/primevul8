MagickExport void GetNextToken(const char *start,const char **end,
  const size_t extent,char *token)
{
  double
    value;

  register const char
    *p;

  register ssize_t
    i;

  size_t
    length;

  assert(start != (const char *) NULL);
  assert(token != (char *) NULL);
  i=0;
  length=strlen(start);
  p=start;
  while ((isspace((int) ((unsigned char) *p)) != 0) && (*p != '\0'))
    p++;
  switch (*p)
  {
    case '\0':
      break;
    case '"':
    case '\'':
    case '`':
    case '{':
    {
      register char
        escape;

      switch (*p)
      {
        case '"': escape='"'; break;
        case '\'': escape='\''; break;
        case '`': escape='\''; break;
        case '{': escape='}'; break;
        default: escape=(*p); break;
      }
      for (p++; *p != '\0'; p++)
      {
        if ((*p == '\\') && ((*(p+1) == escape) || (*(p+1) == '\\')))
          p++;
        else
          if (*p == escape)
            {
              p++;
              break;
            }
        if (i < (ssize_t) (extent-1))
          token[i++]=(*p);
        if ((p-start) >= length)
          break;
      }
      break;
    }
    case '/':
    {
      if (i < (ssize_t) (extent-1))
        token[i++]=(*p++);
      if ((*p == '>') || (*p == '/'))
        if (i < (ssize_t) (extent-1))
          token[i++]=(*p++);
      break;
    }
    default:
    {
      char
        *q;

      value=StringToDouble(p,&q);
      (void) value;
      if ((p != q) && (*p != ','))
        {
          for ( ; (p < q) && (*p != ','); p++)
          {
            if (i < (ssize_t) (extent-1))
              token[i++]=(*p);
            if ((p-start) >= length)
              break;
          }
          if (*p == '%')
            if (i < (ssize_t) (extent-1))
              token[i++]=(*p++);
          break;
        }
      if ((*p != '\0') && (isalpha((int) ((unsigned char) *p)) == 0) &&
          (*p != *DirectorySeparator) && (*p != '#') && (*p != '<'))
        {
          if (i < (ssize_t) (extent-1))
            token[i++]=(*p++);
          break;
        }
      for ( ; *p != '\0'; p++)
      {
        if (((isspace((int) ((unsigned char) *p)) != 0) || (*p == '=') ||
            (*p == ',') || (*p == ':') || (*p == ';')) && (*(p-1) != '\\'))
          break;
        if ((i > 0) && (*p == '<'))
          break;
        if (i < (ssize_t) (extent-1))
          token[i++]=(*p);
        if (*p == '>')
          break;
        if (*p == '(')
          for (p++; *p != '\0'; p++)
          {
            if (i < (ssize_t) (extent-1))
              token[i++]=(*p);
            if ((*p == ')') && (*(p-1) != '\\'))
              break;
            if ((p-start) >= length)
              break;
          }
        if ((p-start) >= length)
          break;
      }
      break;
    }
  }
  token[i]='\0';
  if (LocaleNCompare(token,"url(",4) == 0)
    {
      ssize_t
        offset;

      offset=4;
      if (token[offset] == '#')
        offset++;
      i=(ssize_t) strlen(token);
      (void) CopyMagickString(token,token+offset,MagickPathExtent);
      token[i-offset-1]='\0';
    }
  while (isspace((int) ((unsigned char) *p)) != 0)
    p++;
  if (end != (const char **) NULL)
    *end=(const char *) p;
}