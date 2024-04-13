acl_eval(int where, uschar *s, uschar **user_msgptr, uschar **log_msgptr)
{
address_item adb;
address_item *addr = NULL;
int rc;

*user_msgptr = *log_msgptr = NULL;
sender_verified_failed = NULL;
ratelimiters_cmd = NULL;
log_reject_target = LOG_MAIN|LOG_REJECT;

if (where == ACL_WHERE_RCPT)
  {
  adb = address_defaults;
  addr = &adb;
  addr->address = expand_string(US"$local_part@$domain");
  addr->domain = deliver_domain;
  addr->local_part = deliver_localpart;
  addr->cc_local_part = deliver_localpart;
  addr->lc_local_part = deliver_localpart;
  }

acl_level++;
rc = acl_check_internal(where, addr, s, user_msgptr, log_msgptr);
acl_level--;
return rc;
}