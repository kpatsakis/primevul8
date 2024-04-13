write_xauth (char *number, FILE *output)
{
  Xauth *xa, local_xa;
  char *filename;
  FILE *f;
  struct utsname unames;

  if (uname (&unames))
    {
      g_warning ("uname failed");
      return;
    }

  filename = XauFileName ();
  f = fopen (filename, "rb");
  if (f == NULL)
    return;

  while (TRUE)
    {
      xa = XauReadAuth (f);
      if (xa == NULL)
        break;
      if (xa->family == FamilyLocal &&
          auth_streq (unames.nodename, xa->address, xa->address_length) &&
          (xa->number == NULL || auth_streq (number, xa->number, xa->number_length)))
        {
          local_xa = *xa;
          if (local_xa.number)
            {
              local_xa.number = "99";
              local_xa.number_length = 2;
            }

          if (!XauWriteAuth (output, &local_xa))
            g_warning ("xauth write error");
        }

      XauDisposeAuth (xa);
    }

  fclose (f);
}