acl_verify_reverse(uschar **user_msgptr, uschar **log_msgptr)
{
int rc;

user_msgptr = user_msgptr;  /* stop compiler warning */

/* Previous success */

if (sender_host_name != NULL) return OK;

/* Previous failure */

if (host_lookup_failed)
  {
  *log_msgptr = string_sprintf("host lookup failed%s", host_lookup_msg);
  return FAIL;
  }

/* Need to do a lookup */

HDEBUG(D_acl)
  debug_printf_indent("looking up host name to force name/address consistency check\n");

if ((rc = host_name_lookup()) != OK)
  {
  *log_msgptr = (rc == DEFER)?
    US"host lookup deferred for reverse lookup check"
    :
    string_sprintf("host lookup failed for reverse lookup check%s",
      host_lookup_msg);
  return rc;    /* DEFER or FAIL */
  }

host_build_sender_fullhost();
return OK;
}