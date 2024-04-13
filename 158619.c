smtp_verify_helo(void)
{
BOOL yield = TRUE;

HDEBUG(D_receive) debug_printf("verifying EHLO/HELO argument \"%s\"\n",
  sender_helo_name);

if (sender_helo_name == NULL)
  {
  HDEBUG(D_receive) debug_printf("no EHLO/HELO command was issued\n");
  }

/* Deal with the case of -bs without an IP address */

else if (sender_host_address == NULL)
  {
  HDEBUG(D_receive) debug_printf("no client IP address: assume success\n");
  f.helo_verified = TRUE;
  }

/* Deal with the more common case when there is a sending IP address */

else if (sender_helo_name[0] == '[')
  {
  f.helo_verified = Ustrncmp(sender_helo_name+1, sender_host_address,
    Ustrlen(sender_host_address)) == 0;

#if HAVE_IPV6
  if (!f.helo_verified)
    {
    if (strncmpic(sender_host_address, US"::ffff:", 7) == 0)
      f.helo_verified = Ustrncmp(sender_helo_name + 1,
        sender_host_address + 7, Ustrlen(sender_host_address) - 7) == 0;
    }
#endif

  HDEBUG(D_receive)
    { if (f.helo_verified) debug_printf("matched host address\n"); }
  }

/* Do a reverse lookup if one hasn't already given a positive or negative
response. If that fails, or the name doesn't match, try checking with a forward
lookup. */

else
  {
  if (sender_host_name == NULL && !host_lookup_failed)
    yield = host_name_lookup() != DEFER;

  /* If a host name is known, check it and all its aliases. */

  if (sender_host_name)
    if ((f.helo_verified = strcmpic(sender_host_name, sender_helo_name) == 0))
      {
      sender_helo_dnssec = sender_host_dnssec;
      HDEBUG(D_receive) debug_printf("matched host name\n");
      }
    else
      {
      uschar **aliases = sender_host_aliases;
      while (*aliases)
        if ((f.helo_verified = strcmpic(*aliases++, sender_helo_name) == 0))
	  {
	  sender_helo_dnssec = sender_host_dnssec;
	  break;
	  }

      HDEBUG(D_receive) if (f.helo_verified)
          debug_printf("matched alias %s\n", *(--aliases));
      }

  /* Final attempt: try a forward lookup of the helo name */

  if (!f.helo_verified)
    {
    int rc;
    host_item h;
    dnssec_domains d;
    host_item *hh;

    h.name = sender_helo_name;
    h.address = NULL;
    h.mx = MX_NONE;
    h.next = NULL;
    d.request = US"*";
    d.require = US"";

    HDEBUG(D_receive) debug_printf("getting IP address for %s\n",
      sender_helo_name);
    rc = host_find_bydns(&h, NULL, HOST_FIND_BY_A | HOST_FIND_BY_AAAA,
			  NULL, NULL, NULL, &d, NULL, NULL);
    if (rc == HOST_FOUND || rc == HOST_FOUND_LOCAL)
      for (hh = &h; hh; hh = hh->next)
        if (Ustrcmp(hh->address, sender_host_address) == 0)
          {
          f.helo_verified = TRUE;
	  if (h.dnssec == DS_YES) sender_helo_dnssec = TRUE;
          HDEBUG(D_receive)
	    {
            debug_printf("IP address for %s matches calling address\n"
	      "Forward DNS security status: %sverified\n",
              sender_helo_name, sender_helo_dnssec ? "" : "un");
	    }
          break;
          }
    }
  }

if (!f.helo_verified) f.helo_verify_failed = TRUE;  /* We've tried ... */
return yield;
}