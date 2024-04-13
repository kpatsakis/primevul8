smtp_handle_acl_fail(int where, int rc, uschar *user_msg, uschar *log_msg)
{
BOOL drop = rc == FAIL_DROP;
int codelen = 3;
uschar *smtp_code;
uschar *lognl;
uschar *sender_info = US"";
uschar *what =
#ifdef WITH_CONTENT_SCAN
  where == ACL_WHERE_MIME ? US"during MIME ACL checks" :
#endif
  where == ACL_WHERE_PREDATA ? US"DATA" :
  where == ACL_WHERE_DATA ? US"after DATA" :
#ifndef DISABLE_PRDR
  where == ACL_WHERE_PRDR ? US"after DATA PRDR" :
#endif
  smtp_cmd_data ?
    string_sprintf("%s %s", acl_wherenames[where], smtp_cmd_data) :
    string_sprintf("%s in \"connect\" ACL", acl_wherenames[where]);

if (drop) rc = FAIL;

/* Set the default SMTP code, and allow a user message to change it. */

smtp_code = rc == FAIL ? acl_wherecodes[where] : US"451";
smtp_message_code(&smtp_code, &codelen, &user_msg, &log_msg,
  where != ACL_WHERE_VRFY);

/* We used to have sender_address here; however, there was a bug that was not
updating sender_address after a rewrite during a verify. When this bug was
fixed, sender_address at this point became the rewritten address. I'm not sure
this is what should be logged, so I've changed to logging the unrewritten
address to retain backward compatibility. */

#ifndef WITH_CONTENT_SCAN
if (where == ACL_WHERE_RCPT || where == ACL_WHERE_DATA)
#else
if (where == ACL_WHERE_RCPT || where == ACL_WHERE_DATA || where == ACL_WHERE_MIME)
#endif
  {
  sender_info = string_sprintf("F=<%s>%s%s%s%s ",
    sender_address_unrewritten ? sender_address_unrewritten : sender_address,
    sender_host_authenticated ? US" A="                                    : US"",
    sender_host_authenticated ? sender_host_authenticated                  : US"",
    sender_host_authenticated && authenticated_id ? US":"                  : US"",
    sender_host_authenticated && authenticated_id ? authenticated_id       : US""
    );
  }

/* If there's been a sender verification failure with a specific message, and
we have not sent a response about it yet, do so now, as a preliminary line for
failures, but not defers. However, always log it for defer, and log it for fail
unless the sender_verify_fail log selector has been turned off. */

if (sender_verified_failed &&
    !testflag(sender_verified_failed, af_sverify_told))
  {
  BOOL save_rcpt_in_progress = fl.rcpt_in_progress;
  fl.rcpt_in_progress = FALSE;  /* So as not to treat these as the error */

  setflag(sender_verified_failed, af_sverify_told);

  if (rc != FAIL || LOGGING(sender_verify_fail))
    log_write(0, LOG_MAIN|LOG_REJECT, "%s sender verify %s for <%s>%s",
      host_and_ident(TRUE),
      ((sender_verified_failed->special_action & 255) == DEFER)? "defer":"fail",
      sender_verified_failed->address,
      (sender_verified_failed->message == NULL)? US"" :
      string_sprintf(": %s", sender_verified_failed->message));

  if (rc == FAIL && sender_verified_failed->user_message)
    smtp_respond(smtp_code, codelen, FALSE, string_sprintf(
        testflag(sender_verified_failed, af_verify_pmfail)?
          "Postmaster verification failed while checking <%s>\n%s\n"
          "Several RFCs state that you are required to have a postmaster\n"
          "mailbox for each mail domain. This host does not accept mail\n"
          "from domains whose servers reject the postmaster address."
          :
        testflag(sender_verified_failed, af_verify_nsfail)?
          "Callback setup failed while verifying <%s>\n%s\n"
          "The initial connection, or a HELO or MAIL FROM:<> command was\n"
          "rejected. Refusing MAIL FROM:<> does not help fight spam, disregards\n"
          "RFC requirements, and stops you from receiving standard bounce\n"
          "messages. This host does not accept mail from domains whose servers\n"
          "refuse bounces."
          :
          "Verification failed for <%s>\n%s",
        sender_verified_failed->address,
        sender_verified_failed->user_message));

  fl.rcpt_in_progress = save_rcpt_in_progress;
  }

/* Sort out text for logging */

log_msg = log_msg ? string_sprintf(": %s", log_msg) : US"";
if ((lognl = Ustrchr(log_msg, '\n'))) *lognl = 0;

/* Send permanent failure response to the command, but the code used isn't
always a 5xx one - see comments at the start of this function. If the original
rc was FAIL_DROP we drop the connection and yield 2. */

if (rc == FAIL)
  smtp_respond(smtp_code, codelen, TRUE,
    user_msg ? user_msg : US"Administrative prohibition");

/* Send temporary failure response to the command. Don't give any details,
unless acl_temp_details is set. This is TRUE for a callout defer, a "defer"
verb, and for a header verify when smtp_return_error_details is set.

This conditional logic is all somewhat of a mess because of the odd
interactions between temp_details and return_error_details. One day it should
be re-implemented in a tidier fashion. */

else
  if (f.acl_temp_details && user_msg)
    {
    if (  smtp_return_error_details
       && sender_verified_failed
       && sender_verified_failed->message
       )
      smtp_respond(smtp_code, codelen, FALSE, sender_verified_failed->message);

    smtp_respond(smtp_code, codelen, TRUE, user_msg);
    }
  else
    smtp_respond(smtp_code, codelen, TRUE,
      US"Temporary local problem - please try later");

/* Log the incident to the logs that are specified by log_reject_target
(default main, reject). This can be empty to suppress logging of rejections. If
the connection is not forcibly to be dropped, return 0. Otherwise, log why it
is closing if required and return 2.  */

if (log_reject_target != 0)
  {
#ifdef SUPPORT_TLS
  gstring * g = s_tlslog(NULL);
  uschar * tls = string_from_gstring(g);
  if (!tls) tls = US"";
#else
  uschar * tls = US"";
#endif
  log_write(where == ACL_WHERE_CONNECT ? L_connection_reject : 0,
    log_reject_target, "%s%s%s %s%srejected %s%s",
    LOGGING(dnssec) && sender_host_dnssec ? US" DS" : US"",
    host_and_ident(TRUE),
    tls,
    sender_info,
    rc == FAIL ? US"" : US"temporarily ",
    what, log_msg);
  }

if (!drop) return 0;

log_write(L_smtp_connection, LOG_MAIN, "%s closed by DROP in ACL",
  smtp_get_connection_info());

/* Run the not-quit ACL, but without any custom messages. This should not be a
problem, because we get here only if some other ACL has issued "drop", and
in that case, *its* custom messages will have been used above. */

smtp_notquit_exit(US"acl-drop", NULL, NULL);
return 2;
}