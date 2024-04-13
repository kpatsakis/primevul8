authorize_user_token_ldap (struct cfg *cfg,
			   const char *user,
			   const char *token_id)
{
  DBG(("called"));
  int retval = 0;
  int protocol;
#ifdef HAVE_LIBLDAP
  LDAP *ld = NULL;
  LDAPMessage *result = NULL, *e;
  BerElement *ber;
  char *a;
  char *attrs[2] = {NULL, NULL};

  struct berval **vals;
  int i, rc;

  char *find = NULL, *sr = NULL;

  if (cfg->user_attr == NULL) {
    DBG (("Trying to look up user to YubiKey mapping in LDAP, but user_attr not set!"));
    return 0;
  }
  if (cfg->yubi_attr == NULL) {
    DBG (("Trying to look up user to YubiKey mapping in LDAP, but yubi_attr not set!"));
    return 0;
  }
  if (cfg->ldapdn == NULL) {
    DBG (("Trying to look up user to YubiKey mapping in LDAP, but ldapdn not set!"));
    return 0;
  }

  /* Get a handle to an LDAP connection. */
  if (cfg->ldap_uri)
    {
      rc = ldap_initialize (&ld, cfg->ldap_uri);
      if (rc != LDAP_SUCCESS)
	{
	  DBG (("ldap_init: %s", ldap_err2string (rc)));
	  retval = 0;
	  goto done;
	}
    }
  else
    {
      if ((ld = ldap_init (cfg->ldapserver, PORT_NUMBER)) == NULL)
	{
	  DBG (("ldap_init"));
	  retval = 0;
	  goto done;
	}
    }

  /* LDAPv2 is historical -- RFC3494. */
  protocol = LDAP_VERSION3;
  ldap_set_option (ld, LDAP_OPT_PROTOCOL_VERSION, &protocol);

  /* Bind anonymously to the LDAP server. */
  rc = ldap_simple_bind_s (ld, NULL, NULL);
  if (rc != LDAP_SUCCESS)
    {
      DBG (("ldap_simple_bind_s: %s", ldap_err2string (rc)));
      retval = 0;
      goto done;
    }

  /* Allocation of memory for search strings depending on input size */
  find = malloc((strlen(cfg->user_attr)+strlen(cfg->ldapdn)+strlen(user)+3)*sizeof(char));

  sprintf (find, "%s=%s,%s", cfg->user_attr, user, cfg->ldapdn);

  attrs[0] = (char *) cfg->yubi_attr;

  DBG(("LDAP : look up object '%s', ask for attribute '%s'", find, cfg->yubi_attr));

  /* Search for the entry. */
  if ((rc = ldap_search_ext_s (ld, find, LDAP_SCOPE_BASE,
			       NULL, attrs, 0, NULL, NULL, LDAP_NO_LIMIT,
			       LDAP_NO_LIMIT, &result)) != LDAP_SUCCESS)
    {
      DBG (("ldap_search_ext_s: %s", ldap_err2string (rc)));

      retval = 0;
      goto done;
    }

  e = ldap_first_entry (ld, result);
  if (e == NULL)
    {
      DBG (("No result from LDAP search"));
    }
  else
    {
      /* Iterate through each returned attribute. */
      for (a = ldap_first_attribute (ld, e, &ber);
	   a != NULL; a = ldap_next_attribute (ld, e, ber))
	{
	  if ((vals = ldap_get_values_len (ld, e, a)) != NULL)
	    {
	      /* Compare each value for the attribute against the token id. */
	      for (i = 0; vals[i] != NULL; i++)
		{
		  if (!strncmp (token_id, vals[i]->bv_val, strlen (token_id)))
		    {
		      DBG (("Token Found :: %s", vals[i]->bv_val));
		      retval = 1;
		    }
		  else
		    {
		      DBG (("No match : (%s) %s != %s", a, vals[i]->bv_val, token_id));
		    }
		}
	      ldap_value_free_len (vals);
	    }
	  ldap_memfree (a);
	}
      if (ber != NULL)
	  ber_free (ber, 0);
    }

 done:
  if (result != NULL)
    ldap_msgfree (result);
  if (ld != NULL)
    ldap_unbind (ld);

  /* free memory allocated for search strings */
  if (find != NULL)
    free(find);
  if (sr != NULL)
    free(sr);

#else
  DBG (("Trying to use LDAP, but this function is not compiled in pam_yubico!!"));
  DBG (("Install libldap-dev and then recompile pam_yubico."));
#endif
  return retval;
}