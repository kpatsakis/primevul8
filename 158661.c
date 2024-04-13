decode_bits(unsigned int *selector, size_t selsize, int *notall,
  uschar *string, bit_table *options, int count, uschar *which, int flags)
{
uschar *errmsg;
if (string == NULL) return;

if (*string == '=')
  {
  char *end;    /* Not uschar */
  memset(selector, 0, sizeof(*selector)*selsize);
  *selector = strtoul(CS string+1, &end, 0);
  if (*end == 0) return;
  errmsg = string_sprintf("malformed numeric %s_selector setting: %s", which,
    string);
  goto ERROR_RETURN;
  }

/* Handle symbolic setting */

else for(;;)
  {
  BOOL adding;
  uschar *s;
  int len;
  bit_table *start, *end;

  while (isspace(*string)) string++;
  if (*string == 0) return;

  if (*string != '+' && *string != '-')
    {
    errmsg = string_sprintf("malformed %s_selector setting: "
      "+ or - expected but found \"%s\"", which, string);
    goto ERROR_RETURN;
    }

  adding = *string++ == '+';
  s = string;
  while (isalnum(*string) || *string == '_') string++;
  len = string - s;

  start = options;
  end = options + count;

  while (start < end)
    {
    bit_table *middle = start + (end - start)/2;
    int c = Ustrncmp(s, middle->name, len);
    if (c == 0)
      {
      if (middle->name[len] != 0) c = -1; else
        {
        unsigned int bit = middle->bit;

	if (bit == -1)
	  {
	  if (adding)
	    {
	    memset(selector, -1, sizeof(*selector)*selsize);
	    bits_clear(selector, selsize, notall);
	    }
	  else
	    memset(selector, 0, sizeof(*selector)*selsize);
	  }
	else if (adding)
	  BIT_SET(selector, selsize, bit);
	else
	  BIT_CLEAR(selector, selsize, bit);

        break;  /* Out of loop to match selector name */
        }
      }
    if (c < 0) end = middle; else start = middle + 1;
    }  /* Loop to match selector name */

  if (start >= end)
    {
    errmsg = string_sprintf("unknown %s_selector setting: %c%.*s", which,
      adding? '+' : '-', len, s);
    goto ERROR_RETURN;
    }
  }    /* Loop for selector names */

/* Handle disasters */

ERROR_RETURN:
if (Ustrcmp(which, "debug") == 0)
  {
  if (flags & DEBUG_FROM_CONFIG)
    {
    log_write(0, LOG_CONFIG|LOG_PANIC, "%s", errmsg);
    return;
    }
  fprintf(stderr, "exim: %s\n", errmsg);
  exit(EXIT_FAILURE);
  }
else log_write(0, LOG_CONFIG|LOG_PANIC_DIE, "%s", errmsg);
}