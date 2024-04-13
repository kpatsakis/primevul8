smtp_in_auth(auth_instance *au, uschar ** s, uschar ** ss)
{
const uschar *set_id = NULL;
int rc, i;

/* Run the checking code, passing the remainder of the command line as
data. Initials the $auth<n> variables as empty. Initialize $0 empty and set
it as the only set numerical variable. The authenticator may set $auth<n>
and also set other numeric variables. The $auth<n> variables are preferred
nowadays; the numerical variables remain for backwards compatibility.

Afterwards, have a go at expanding the set_id string, even if
authentication failed - for bad passwords it can be useful to log the
userid. On success, require set_id to expand and exist, and put it in
authenticated_id. Save this in permanent store, as the working store gets
reset at HELO, RSET, etc. */

for (i = 0; i < AUTH_VARS; i++) auth_vars[i] = NULL;
expand_nmax = 0;
expand_nlength[0] = 0;   /* $0 contains nothing */

rc = (au->info->servercode)(au, smtp_cmd_data);
if (au->set_id) set_id = expand_string(au->set_id);
expand_nmax = -1;        /* Reset numeric variables */
for (i = 0; i < AUTH_VARS; i++) auth_vars[i] = NULL;   /* Reset $auth<n> */

/* The value of authenticated_id is stored in the spool file and printed in
log lines. It must not contain binary zeros or newline characters. In
normal use, it never will, but when playing around or testing, this error
can (did) happen. To guard against this, ensure that the id contains only
printing characters. */

if (set_id) set_id = string_printing(set_id);

/* For the non-OK cases, set up additional logging data if set_id
is not empty. */

if (rc != OK)
  set_id = set_id && *set_id
    ? string_sprintf(" (set_id=%s)", set_id) : US"";

/* Switch on the result */

switch(rc)
  {
  case OK:
  if (!au->set_id || set_id)    /* Complete success */
    {
    if (set_id) authenticated_id = string_copy_malloc(set_id);
    sender_host_authenticated = au->name;
    sender_host_auth_pubname  = au->public_name;
    authentication_failed = FALSE;
    authenticated_fail_id = NULL;   /* Impossible to already be set? */

    received_protocol =
      (sender_host_address ? protocols : protocols_local)
	[pextend + pauthed + (tls_in.active.sock >= 0 ? pcrpted:0)];
    *s = *ss = US"235 Authentication succeeded";
    authenticated_by = au;
    break;
    }

  /* Authentication succeeded, but we failed to expand the set_id string.
  Treat this as a temporary error. */

  auth_defer_msg = expand_string_message;
  /* Fall through */

  case DEFER:
  if (set_id) authenticated_fail_id = string_copy_malloc(set_id);
  *s = string_sprintf("435 Unable to authenticate at present%s",
    auth_defer_user_msg);
  *ss = string_sprintf("435 Unable to authenticate at present%s: %s",
    set_id, auth_defer_msg);
  break;

  case BAD64:
  *s = *ss = US"501 Invalid base64 data";
  break;

  case CANCELLED:
  *s = *ss = US"501 Authentication cancelled";
  break;

  case UNEXPECTED:
  *s = *ss = US"553 Initial data not expected";
  break;

  case FAIL:
  if (set_id) authenticated_fail_id = string_copy_malloc(set_id);
  *s = US"535 Incorrect authentication data";
  *ss = string_sprintf("535 Incorrect authentication data%s", set_id);
  break;

  default:
  if (set_id) authenticated_fail_id = string_copy_malloc(set_id);
  *s = US"435 Internal error";
  *ss = string_sprintf("435 Internal error%s: return %d from authentication "
    "check", set_id, rc);
  break;
  }

return rc;
}