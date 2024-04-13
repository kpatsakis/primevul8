acl_getline(void)
{
uschar *yield;

/* This loop handles leading blank lines and comments. */

for(;;)
  {
  while (isspace(*acl_text)) acl_text++;   /* Leading spaces/empty lines */
  if (*acl_text == 0) return NULL;         /* No more data */
  yield = acl_text;                        /* Potential data line */

  while (*acl_text != 0 && *acl_text != '\n') acl_text++;

  /* If we hit the end before a newline, we have the whole logical line. If
  it's a comment, there's no more data to be given. Otherwise, yield it. */

  if (*acl_text == 0) return (*yield == '#')? NULL : yield;

  /* After reaching a newline, end this loop if the physical line does not
  start with '#'. If it does, it's a comment, and the loop continues. */

  if (*yield != '#') break;
  }

/* This loop handles continuations. We know we have some real data, ending in
newline. See if there is a continuation marker at the end (ignoring trailing
white space). We know that *yield is not white space, so no need to test for
cont > yield in the backwards scanning loop. */

for(;;)
  {
  uschar *cont;
  for (cont = acl_text - 1; isspace(*cont); cont--);

  /* If no continuation follows, we are done. Mark the end of the line and
  return it. */

  if (*cont != '\\')
    {
    *acl_text++ = 0;
    return yield;
    }

  /* We have encountered a continuation. Skip over whitespace at the start of
  the next line, and indeed the whole of the next line or lines if they are
  comment lines. */

  for (;;)
    {
    while (*(++acl_text) == ' ' || *acl_text == '\t');
    if (*acl_text != '#') break;
    while (*(++acl_text) != 0 && *acl_text != '\n');
    }

  /* We have the start of a continuation line. Move all the rest of the data
  to join onto the previous line, and then find its end. If the end is not a
  newline, we are done. Otherwise loop to look for another continuation. */

  memmove(cont, acl_text, acl_text_end - acl_text);
  acl_text_end -= acl_text - cont;
  acl_text = cont;
  while (*acl_text != 0 && *acl_text != '\n') acl_text++;
  if (*acl_text == 0) return yield;
  }

/* Control does not reach here */
}