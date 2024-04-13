acl_verify(int where, address_item *addr, const uschar *arg,
  uschar **user_msgptr, uschar **log_msgptr, int *basic_errno)
{
int sep = '/';
int callout = -1;
int callout_overall = -1;
int callout_connect = -1;
int verify_options = 0;
int rc;
BOOL verify_header_sender = FALSE;
BOOL defer_ok = FALSE;
BOOL callout_defer_ok = FALSE;
BOOL no_details = FALSE;
BOOL success_on_redirect = FALSE;
address_item *sender_vaddr = NULL;
uschar *verify_sender_address = NULL;
uschar *pm_mailfrom = NULL;
uschar *se_mailfrom = NULL;

/* Some of the verify items have slash-separated options; some do not. Diagnose
an error if options are given for items that don't expect them.
*/

uschar *slash = Ustrchr(arg, '/');
const uschar *list = arg;
uschar *ss = string_nextinlist(&list, &sep, big_buffer, big_buffer_size);
verify_type_t * vp;

if (!ss) goto BAD_VERIFY;

/* Handle name/address consistency verification in a separate function. */

for (vp= verify_type_list;
     CS vp < CS verify_type_list + sizeof(verify_type_list);
     vp++
    )
  if (vp->alt_opt_sep ? strncmpic(ss, vp->name, vp->alt_opt_sep) == 0
                      : strcmpic (ss, vp->name) == 0)
   break;
if (CS vp >= CS verify_type_list + sizeof(verify_type_list))
  goto BAD_VERIFY;

if (vp->no_options && slash)
  {
  *log_msgptr = string_sprintf("unexpected '/' found in \"%s\" "
    "(this verify item has no options)", arg);
  return ERROR;
  }
if (!(vp->where_allowed & BIT(where)))
  {
  *log_msgptr = string_sprintf("cannot verify %s in ACL for %s",
		  vp->name, acl_wherenames[where]);
  return ERROR;
  }
switch(vp->value)
  {
  case VERIFY_REV_HOST_LKUP:
    if (!sender_host_address) return OK;
    if ((rc = acl_verify_reverse(user_msgptr, log_msgptr)) == DEFER)
      while ((ss = string_nextinlist(&list, &sep, NULL, 0)))
	if (strcmpic(ss, US"defer_ok") == 0)
	  return OK;
    return rc;

  case VERIFY_CERT:
    /* TLS certificate verification is done at STARTTLS time; here we just
    test whether it was successful or not. (This is for optional verification; for
    mandatory verification, the connection doesn't last this long.) */

    if (tls_in.certificate_verified) return OK;
    *user_msgptr = US"no verified certificate";
    return FAIL;

  case VERIFY_HELO:
    /* We can test the result of optional HELO verification that might have
    occurred earlier. If not, we can attempt the verification now. */

    if (!f.helo_verified && !f.helo_verify_failed) smtp_verify_helo();
    return f.helo_verified ? OK : FAIL;

  case VERIFY_CSA:
    /* Do Client SMTP Authorization checks in a separate function, and turn the
    result code into user-friendly strings. */

    rc = acl_verify_csa(list);
    *log_msgptr = *user_msgptr = string_sprintf("client SMTP authorization %s",
                                              csa_reason_string[rc]);
    csa_status = csa_status_string[rc];
    DEBUG(D_acl) debug_printf_indent("CSA result %s\n", csa_status);
    return csa_return_code[rc];

#ifdef EXPERIMENTAL_ARC
  case VERIFY_ARC:
    {	/* Do Authenticated Received Chain checks in a separate function. */
    const uschar * condlist = CUS string_nextinlist(&list, &sep, NULL, 0);
    int csep = 0;
    uschar * cond;

    if (!(arc_state = acl_verify_arc())) return DEFER;
    DEBUG(D_acl) debug_printf_indent("ARC verify result %s %s%s%s\n", arc_state,
      arc_state_reason ? "(":"", arc_state_reason, arc_state_reason ? ")":"");

    if (!condlist) condlist = US"none:pass";
    while ((cond = string_nextinlist(&condlist, &csep, NULL, 0)))
      if (Ustrcmp(arc_state, cond) == 0) return OK;
    return FAIL;
    }
#endif

  case VERIFY_HDR_SYNTAX:
    /* Check that all relevant header lines have the correct 5322-syntax. If there is
    a syntax error, we return details of the error to the sender if configured to
    send out full details. (But a "message" setting on the ACL can override, as
    always). */

    rc = verify_check_headers(log_msgptr);
    if (rc != OK && *log_msgptr)
      if (smtp_return_error_details)
	*user_msgptr = string_sprintf("Rejected after DATA: %s", *log_msgptr);
      else
	acl_verify_message = *log_msgptr;
    return rc;

  case VERIFY_HDR_NAMES_ASCII:
    /* Check that all header names are true 7 bit strings
    See RFC 5322, 2.2. and RFC 6532, 3. */

    rc = verify_check_header_names_ascii(log_msgptr);
    if (rc != OK && smtp_return_error_details && *log_msgptr)
      *user_msgptr = string_sprintf("Rejected after DATA: %s", *log_msgptr);
    return rc;

  case VERIFY_NOT_BLIND:
    /* Check that no recipient of this message is "blind", that is, every envelope
    recipient must be mentioned in either To: or Cc:. */

    if ((rc = verify_check_notblind()) != OK)
      {
      *log_msgptr = string_sprintf("bcc recipient detected");
      if (smtp_return_error_details)
        *user_msgptr = string_sprintf("Rejected after DATA: %s", *log_msgptr);
      }
    return rc;

  /* The remaining verification tests check recipient and sender addresses,
  either from the envelope or from the header. There are a number of
  slash-separated options that are common to all of them. */

  case VERIFY_HDR_SNDR:
    verify_header_sender = TRUE;
    break;

  case VERIFY_SNDR:
    /* In the case of a sender, this can optionally be followed by an address to use
    in place of the actual sender (rare special-case requirement). */
    {
    uschar *s = ss + 6;
    if (*s == 0)
      verify_sender_address = sender_address;
    else
      {
      while (isspace(*s)) s++;
      if (*s++ != '=') goto BAD_VERIFY;
      while (isspace(*s)) s++;
      verify_sender_address = string_copy(s);
      }
    }
    break;

  case VERIFY_RCPT:
    break;
  }



/* Remaining items are optional; they apply to sender and recipient
verification, including "header sender" verification. */

while ((ss = string_nextinlist(&list, &sep, big_buffer, big_buffer_size))
      != NULL)
  {
  if (strcmpic(ss, US"defer_ok") == 0) defer_ok = TRUE;
  else if (strcmpic(ss, US"no_details") == 0) no_details = TRUE;
  else if (strcmpic(ss, US"success_on_redirect") == 0) success_on_redirect = TRUE;

  /* These two old options are left for backwards compatibility */

  else if (strcmpic(ss, US"callout_defer_ok") == 0)
    {
    callout_defer_ok = TRUE;
    if (callout == -1) callout = CALLOUT_TIMEOUT_DEFAULT;
    }

  else if (strcmpic(ss, US"check_postmaster") == 0)
     {
     pm_mailfrom = US"";
     if (callout == -1) callout = CALLOUT_TIMEOUT_DEFAULT;
     }

  /* The callout option has a number of sub-options, comma separated */

  else if (strncmpic(ss, US"callout", 7) == 0)
    {
    callout = CALLOUT_TIMEOUT_DEFAULT;
    ss += 7;
    if (*ss != 0)
      {
      while (isspace(*ss)) ss++;
      if (*ss++ == '=')
        {
	const uschar * sublist = ss;
        int optsep = ',';
        uschar *opt;
        uschar buffer[256];
        while (isspace(*sublist)) sublist++;

        while ((opt = string_nextinlist(&sublist, &optsep, buffer, sizeof(buffer))))
          {
	  callout_opt_t * op;
	  double period = 1.0F;

	  for (op= callout_opt_list; op->name; op++)
	    if (strncmpic(opt, op->name, Ustrlen(op->name)) == 0)
	      break;

	  verify_options |= op->flag;
	  if (op->has_option)
	    {
	    opt += Ustrlen(op->name);
            while (isspace(*opt)) opt++;
            if (*opt++ != '=')
              {
              *log_msgptr = string_sprintf("'=' expected after "
                "\"%s\" in ACL verify condition \"%s\"", op->name, arg);
              return ERROR;
              }
            while (isspace(*opt)) opt++;
	    }
	  if (op->timeval && (period = readconf_readtime(opt, 0, FALSE)) < 0)
	    {
	    *log_msgptr = string_sprintf("bad time value in ACL condition "
	      "\"verify %s\"", arg);
	    return ERROR;
	    }

	  switch(op->value)
	    {
            case CALLOUT_DEFER_OK:		callout_defer_ok = TRUE; break;
            case CALLOUT_POSTMASTER:		pm_mailfrom = US"";	break;
            case CALLOUT_FULLPOSTMASTER:	pm_mailfrom = US"";	break;
            case CALLOUT_MAILFROM:
              if (!verify_header_sender)
                {
                *log_msgptr = string_sprintf("\"mailfrom\" is allowed as a "
                  "callout option only for verify=header_sender (detected in ACL "
                  "condition \"%s\")", arg);
                return ERROR;
                }
              se_mailfrom = string_copy(opt);
  	      break;
            case CALLOUT_POSTMASTER_MAILFROM:	pm_mailfrom = string_copy(opt); break;
            case CALLOUT_MAXWAIT:		callout_overall = period;	break;
            case CALLOUT_CONNECT:		callout_connect = period;	break;
            case CALLOUT_TIME:			callout = period;		break;
	    }
          }
        }
      else
        {
        *log_msgptr = string_sprintf("'=' expected after \"callout\" in "
          "ACL condition \"%s\"", arg);
        return ERROR;
        }
      }
    }

  /* Option not recognized */

  else
    {
    *log_msgptr = string_sprintf("unknown option \"%s\" in ACL "
      "condition \"verify %s\"", ss, arg);
    return ERROR;
    }
  }

if ((verify_options & (vopt_callout_recipsender|vopt_callout_recippmaster)) ==
      (vopt_callout_recipsender|vopt_callout_recippmaster))
  {
  *log_msgptr = US"only one of use_sender and use_postmaster can be set "
    "for a recipient callout";
  return ERROR;
  }

/* Handle sender-in-header verification. Default the user message to the log
message if giving out verification details. */

if (verify_header_sender)
  {
  int verrno;

  if ((rc = verify_check_header_address(user_msgptr, log_msgptr, callout,
    callout_overall, callout_connect, se_mailfrom, pm_mailfrom, verify_options,
    &verrno)) != OK)
    {
    *basic_errno = verrno;
    if (smtp_return_error_details)
      {
      if (!*user_msgptr && *log_msgptr)
        *user_msgptr = string_sprintf("Rejected after DATA: %s", *log_msgptr);
      if (rc == DEFER) f.acl_temp_details = TRUE;
      }
    }
  }

/* Handle a sender address. The default is to verify *the* sender address, but
optionally a different address can be given, for special requirements. If the
address is empty, we are dealing with a bounce message that has no sender, so
we cannot do any checking. If the real sender address gets rewritten during
verification (e.g. DNS widening), set the flag to stop it being rewritten again
during message reception.

A list of verified "sender" addresses is kept to try to avoid doing to much
work repetitively when there are multiple recipients in a message and they all
require sender verification. However, when callouts are involved, it gets too
complicated because different recipients may require different callout options.
Therefore, we always do a full sender verify when any kind of callout is
specified. Caching elsewhere, for instance in the DNS resolver and in the
callout handling, should ensure that this is not terribly inefficient. */

else if (verify_sender_address)
  {
  if ((verify_options & (vopt_callout_recipsender|vopt_callout_recippmaster)))
    {
    *log_msgptr = US"use_sender or use_postmaster cannot be used for a "
      "sender verify callout";
    return ERROR;
    }

  sender_vaddr = verify_checked_sender(verify_sender_address);
  if (sender_vaddr != NULL &&               /* Previously checked */
      callout <= 0)                         /* No callout needed this time */
    {
    /* If the "routed" flag is set, it means that routing worked before, so
    this check can give OK (the saved return code value, if set, belongs to a
    callout that was done previously). If the "routed" flag is not set, routing
    must have failed, so we use the saved return code. */

    if (testflag(sender_vaddr, af_verify_routed))
      rc = OK;
    else
      {
      rc = sender_vaddr->special_action;
      *basic_errno = sender_vaddr->basic_errno;
      }
    HDEBUG(D_acl) debug_printf_indent("using cached sender verify result\n");
    }

  /* Do a new verification, and cache the result. The cache is used to avoid
  verifying the sender multiple times for multiple RCPTs when callouts are not
  specified (see comments above).

  The cache is also used on failure to give details in response to the first
  RCPT that gets bounced for this reason. However, this can be suppressed by
  the no_details option, which sets the flag that says "this detail has already
  been sent". The cache normally contains just one address, but there may be
  more in esoteric circumstances. */

  else
    {
    BOOL routed = TRUE;
    uschar *save_address_data = deliver_address_data;

    sender_vaddr = deliver_make_addr(verify_sender_address, TRUE);
#ifdef SUPPORT_I18N
    if ((sender_vaddr->prop.utf8_msg = message_smtputf8))
      {
      sender_vaddr->prop.utf8_downcvt =       message_utf8_downconvert == 1;
      sender_vaddr->prop.utf8_downcvt_maybe = message_utf8_downconvert == -1;
      }
#endif
    if (no_details) setflag(sender_vaddr, af_sverify_told);
    if (verify_sender_address[0] != 0)
      {
      /* If this is the real sender address, save the unrewritten version
      for use later in receive. Otherwise, set a flag so that rewriting the
      sender in verify_address() does not update sender_address. */

      if (verify_sender_address == sender_address)
        sender_address_unrewritten = sender_address;
      else
        verify_options |= vopt_fake_sender;

      if (success_on_redirect)
        verify_options |= vopt_success_on_redirect;

      /* The recipient, qualify, and expn options are never set in
      verify_options. */

      rc = verify_address(sender_vaddr, NULL, verify_options, callout,
        callout_overall, callout_connect, se_mailfrom, pm_mailfrom, &routed);

      HDEBUG(D_acl) debug_printf_indent("----------- end verify ------------\n");

      if (rc != OK)
        *basic_errno = sender_vaddr->basic_errno;
      else
	DEBUG(D_acl)
	  {
	  if (Ustrcmp(sender_vaddr->address, verify_sender_address) != 0)
	    debug_printf_indent("sender %s verified ok as %s\n",
	      verify_sender_address, sender_vaddr->address);
	  else
	    debug_printf_indent("sender %s verified ok\n",
	      verify_sender_address);
	  }
      }
    else
      rc = OK;  /* Null sender */

    /* Cache the result code */

    if (routed) setflag(sender_vaddr, af_verify_routed);
    if (callout > 0) setflag(sender_vaddr, af_verify_callout);
    sender_vaddr->special_action = rc;
    sender_vaddr->next = sender_verified_list;
    sender_verified_list = sender_vaddr;

    /* Restore the recipient address data, which might have been clobbered by
    the sender verification. */

    deliver_address_data = save_address_data;
    }

  /* Put the sender address_data value into $sender_address_data */

  sender_address_data = sender_vaddr->prop.address_data;
  }

/* A recipient address just gets a straightforward verify; again we must handle
the DEFER overrides. */

else
  {
  address_item addr2;

  if (success_on_redirect)
    verify_options |= vopt_success_on_redirect;

  /* We must use a copy of the address for verification, because it might
  get rewritten. */

  addr2 = *addr;
  rc = verify_address(&addr2, NULL, verify_options|vopt_is_recipient, callout,
    callout_overall, callout_connect, se_mailfrom, pm_mailfrom, NULL);
  HDEBUG(D_acl) debug_printf_indent("----------- end verify ------------\n");

  *basic_errno = addr2.basic_errno;
  *log_msgptr = addr2.message;
  *user_msgptr = (addr2.user_message != NULL)?
    addr2.user_message : addr2.message;

  /* Allow details for temporary error if the address is so flagged. */
  if (testflag((&addr2), af_pass_message)) f.acl_temp_details = TRUE;

  /* Make $address_data visible */
  deliver_address_data = addr2.prop.address_data;
  }

/* We have a result from the relevant test. Handle defer overrides first. */

if (rc == DEFER && (defer_ok ||
   (callout_defer_ok && *basic_errno == ERRNO_CALLOUTDEFER)))
  {
  HDEBUG(D_acl) debug_printf_indent("verify defer overridden by %s\n",
    defer_ok? "defer_ok" : "callout_defer_ok");
  rc = OK;
  }

/* If we've failed a sender, set up a recipient message, and point
sender_verified_failed to the address item that actually failed. */

if (rc != OK && verify_sender_address != NULL)
  {
  if (rc != DEFER)
    *log_msgptr = *user_msgptr = US"Sender verify failed";
  else if (*basic_errno != ERRNO_CALLOUTDEFER)
    *log_msgptr = *user_msgptr = US"Could not complete sender verify";
  else
    {
    *log_msgptr = US"Could not complete sender verify callout";
    *user_msgptr = smtp_return_error_details? sender_vaddr->user_message :
      *log_msgptr;
    }

  sender_verified_failed = sender_vaddr;
  }

/* Verifying an address messes up the values of $domain and $local_part,
so reset them before returning if this is a RCPT ACL. */

if (addr != NULL)
  {
  deliver_domain = addr->domain;
  deliver_localpart = addr->local_part;
  }
return rc;

/* Syntax errors in the verify argument come here. */

BAD_VERIFY:
*log_msgptr = string_sprintf("expected \"sender[=address]\", \"recipient\", "
  "\"helo\", \"header_syntax\", \"header_sender\", \"header_names_ascii\" "
  "or \"reverse_host_lookup\" at start of ACL condition "
  "\"verify %s\"", arg);
return ERROR;
}