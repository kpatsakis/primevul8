acl_warn(int where, uschar *user_message, uschar *log_message)
{
if (log_message != NULL && log_message != user_message)
  {
  uschar *text;
  string_item *logged;

  text = string_sprintf("%s Warning: %s",  host_and_ident(TRUE),
    string_printing(log_message));

  /* If a sender verification has failed, and the log message is "sender verify
  failed", add the failure message. */

  if (sender_verified_failed != NULL &&
      sender_verified_failed->message != NULL &&
      strcmpic(log_message, US"sender verify failed") == 0)
    text = string_sprintf("%s: %s", text, sender_verified_failed->message);

  /* Search previously logged warnings. They are kept in malloc
  store so they can be freed at the start of a new message. */

  for (logged = acl_warn_logged; logged != NULL; logged = logged->next)
    if (Ustrcmp(logged->text, text) == 0) break;

  if (logged == NULL)
    {
    int length = Ustrlen(text) + 1;
    log_write(0, LOG_MAIN, "%s", text);
    logged = store_malloc(sizeof(string_item) + length);
    logged->text = US logged + sizeof(string_item);
    memcpy(logged->text, text, length);
    logged->next = acl_warn_logged;
    acl_warn_logged = logged;
    }
  }

/* If there's no user message, we are done. */

if (user_message == NULL) return;

/* If this isn't a message ACL, we can't do anything with a user message.
Log an error. */

if (where > ACL_WHERE_NOTSMTP)
  {
  log_write(0, LOG_MAIN|LOG_PANIC, "ACL \"warn\" with \"message\" setting "
    "found in a non-message (%s) ACL: cannot specify header lines here: "
    "message ignored", acl_wherenames[where]);
  return;
  }

/* The code for setting up header lines is now abstracted into a separate
function so that it can be used for the add_header modifier as well. */

setup_header(user_message);
}