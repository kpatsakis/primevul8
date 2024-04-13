transport_set_up_command(const uschar ***argvptr, uschar *cmd,
  BOOL expand_arguments, int expand_failed, address_item *addr,
  uschar *etext, uschar **errptr)
{
address_item *ad;
const uschar **argv;
uschar *s, *ss;
int address_count = 0;
int argcount = 0;
int i, max_args;

/* Get store in which to build an argument list. Count the number of addresses
supplied, and allow for that many arguments, plus an additional 60, which
should be enough for anybody. Multiple addresses happen only when the local
delivery batch option is set. */

for (ad = addr; ad != NULL; ad = ad->next) address_count++;
max_args = address_count + 60;
*argvptr = argv = store_get((max_args+1)*sizeof(uschar *));

/* Split the command up into arguments terminated by white space. Lose
trailing space at the start and end. Double-quoted arguments can contain \\ and
\" escapes and so can be handled by the standard function; single-quoted
arguments are verbatim. Copy each argument into a new string. */

s = cmd;
while (isspace(*s)) s++;

while (*s != 0 && argcount < max_args)
  {
  if (*s == '\'')
    {
    ss = s + 1;
    while (*ss != 0 && *ss != '\'') ss++;
    argv[argcount++] = ss = store_get(ss - s++);
    while (*s != 0 && *s != '\'') *ss++ = *s++;
    if (*s != 0) s++;
    *ss++ = 0;
    }
  else argv[argcount++] = string_copy(string_dequote(CUSS &s));
  while (isspace(*s)) s++;
  }

argv[argcount] = US 0;

/* If *s != 0 we have run out of argument slots. */

if (*s != 0)
  {
  uschar *msg = string_sprintf("Too many arguments in command \"%s\" in "
    "%s", cmd, etext);
  if (addr != NULL)
    {
    addr->transport_return = FAIL;
    addr->message = msg;
    }
  else *errptr = msg;
  return FALSE;
  }

/* Expand each individual argument if required. Expansion happens for pipes set
up in filter files and with directly-supplied commands. It does not happen if
the pipe comes from a traditional .forward file. A failing expansion is a big
disaster if the command came from Exim's configuration; if it came from a user
it is just a normal failure. The expand_failed value is used as the error value
to cater for these two cases.

An argument consisting just of the text "$pipe_addresses" is treated specially.
It is not passed to the general expansion function. Instead, it is replaced by
a number of arguments, one for each address. This avoids problems with shell
metacharacters and spaces in addresses.

If the parent of the top address has an original part of "system-filter", this
pipe was set up by the system filter, and we can permit the expansion of
$recipients. */

DEBUG(D_transport)
  {
  debug_printf("direct command:\n");
  for (i = 0; argv[i] != US 0; i++)
    debug_printf("  argv[%d] = %s\n", i, string_printing(argv[i]));
  }

if (expand_arguments)
  {
  BOOL allow_dollar_recipients = addr != NULL &&
    addr->parent != NULL &&
    Ustrcmp(addr->parent->address, "system-filter") == 0;

  for (i = 0; argv[i] != US 0; i++)
    {

    /* Handle special fudge for passing an address list */

    if (addr != NULL &&
        (Ustrcmp(argv[i], "$pipe_addresses") == 0 ||
         Ustrcmp(argv[i], "${pipe_addresses}") == 0))
      {
      int additional;

      if (argcount + address_count - 1 > max_args)
        {
        addr->transport_return = FAIL;
        addr->message = string_sprintf("Too many arguments to command \"%s\" "
          "in %s", cmd, etext);
        return FALSE;
        }

      additional = address_count - 1;
      if (additional > 0)
        memmove(argv + i + 1 + additional, argv + i + 1,
          (argcount - i)*sizeof(uschar *));

      for (ad = addr; ad != NULL; ad = ad->next) {
          argv[i++] = ad->address;
          argcount++;
      }

      /* Subtract one since we replace $pipe_addresses */
      argcount--;
      i--;
      }

      /* Handle special case of $address_pipe when af_force_command is set */

    else if (addr != NULL && testflag(addr,af_force_command) &&
        (Ustrcmp(argv[i], "$address_pipe") == 0 ||
         Ustrcmp(argv[i], "${address_pipe}") == 0))
      {
      int address_pipe_i;
      int address_pipe_argcount = 0;
      int address_pipe_max_args;
      uschar **address_pipe_argv;

      /* We can never have more then the argv we will be loading into */
      address_pipe_max_args = max_args - argcount + 1;

      DEBUG(D_transport)
        debug_printf("address_pipe_max_args=%d\n", address_pipe_max_args);

      /* We allocate an additional for (uschar *)0 */
      address_pipe_argv = store_get((address_pipe_max_args+1)*sizeof(uschar *));

      /* +1 because addr->local_part[0] == '|' since af_force_command is set */
      s = expand_string(addr->local_part + 1);

      if (s == NULL || *s == '\0')
        {
        addr->transport_return = FAIL;
        addr->message = string_sprintf("Expansion of \"%s\" "
           "from command \"%s\" in %s failed: %s",
           (addr->local_part + 1), cmd, etext, expand_string_message);
        return FALSE;
        }

      while (isspace(*s)) s++; /* strip leading space */

      while (*s != 0 && address_pipe_argcount < address_pipe_max_args)
        {
        if (*s == '\'')
          {
          ss = s + 1;
          while (*ss != 0 && *ss != '\'') ss++;
          address_pipe_argv[address_pipe_argcount++] = ss = store_get(ss - s++);
          while (*s != 0 && *s != '\'') *ss++ = *s++;
          if (*s != 0) s++;
          *ss++ = 0;
          }
        else address_pipe_argv[address_pipe_argcount++] =
	      string_copy(string_dequote(CUSS &s));
        while (isspace(*s)) s++; /* strip space after arg */
        }

      address_pipe_argv[address_pipe_argcount] = US 0;

      /* If *s != 0 we have run out of argument slots. */
      if (*s != 0)
        {
        uschar *msg = string_sprintf("Too many arguments in $address_pipe "
          "\"%s\" in %s", addr->local_part + 1, etext);
        if (addr != NULL)
          {
          addr->transport_return = FAIL;
          addr->message = msg;
          }
        else *errptr = msg;
        return FALSE;
        }

      /* address_pipe_argcount - 1
       * because we are replacing $address_pipe in the argument list
       * with the first thing it expands to */
      if (argcount + address_pipe_argcount - 1 > max_args)
        {
        addr->transport_return = FAIL;
        addr->message = string_sprintf("Too many arguments to command "
          "\"%s\" after expanding $address_pipe in %s", cmd, etext);
        return FALSE;
        }

      /* If we are not just able to replace the slot that contained
       * $address_pipe (address_pipe_argcount == 1)
       * We have to move the existing argv by address_pipe_argcount - 1
       * Visually if address_pipe_argcount == 2:
       * [argv 0][argv 1][argv 2($address_pipe)][argv 3][0]
       * [argv 0][argv 1][ap_arg0][ap_arg1][old argv 3][0]
       */
      if (address_pipe_argcount > 1)
        memmove(
          /* current position + additional args */
          argv + i + address_pipe_argcount,
          /* current position + 1 (for the (uschar *)0 at the end) */
          argv + i + 1,
          /* -1 for the (uschar *)0 at the end)*/
          (argcount - i)*sizeof(uschar *)
        );

      /* Now we fill in the slots we just moved argv out of
       * [argv 0][argv 1][argv 2=pipeargv[0]][argv 3=pipeargv[1]][old argv 3][0]
       */
      for (address_pipe_i = 0;
           address_pipe_argv[address_pipe_i] != US 0;
           address_pipe_i++)
        {
        argv[i++] = address_pipe_argv[address_pipe_i];
        argcount++;
        }

      /* Subtract one since we replace $address_pipe */
      argcount--;
      i--;
      }

    /* Handle normal expansion string */

    else
      {
      const uschar *expanded_arg;
      f.enable_dollar_recipients = allow_dollar_recipients;
      expanded_arg = expand_cstring(argv[i]);
      f.enable_dollar_recipients = FALSE;

      if (expanded_arg == NULL)
        {
        uschar *msg = string_sprintf("Expansion of \"%s\" "
          "from command \"%s\" in %s failed: %s",
          argv[i], cmd, etext, expand_string_message);
        if (addr != NULL)
          {
          addr->transport_return = expand_failed;
          addr->message = msg;
          }
        else *errptr = msg;
        return FALSE;
        }
      argv[i] = expanded_arg;
      }
    }

  DEBUG(D_transport)
    {
    debug_printf("direct command after expansion:\n");
    for (i = 0; argv[i] != US 0; i++)
      debug_printf("  argv[%d] = %s\n", i, string_printing(argv[i]));
    }
  }

return TRUE;
}