acl_check_condition(int verb, acl_condition_block *cb, int where,
  address_item *addr, int level, BOOL *epp, uschar **user_msgptr,
  uschar **log_msgptr, int *basic_errno)
{
uschar *user_message = NULL;
uschar *log_message = NULL;
int rc = OK;
#ifdef WITH_CONTENT_SCAN
int sep = -'/';
#endif

for (; cb; cb = cb->next)
  {
  const uschar *arg;
  int control_type;

  /* The message and log_message items set up messages to be used in
  case of rejection. They are expanded later. */

  if (cb->type == ACLC_MESSAGE)
    {
    HDEBUG(D_acl) debug_printf_indent("  message: %s\n", cb->arg);
    user_message = cb->arg;
    continue;
    }

  if (cb->type == ACLC_LOG_MESSAGE)
    {
    HDEBUG(D_acl) debug_printf_indent("l_message: %s\n", cb->arg);
    log_message = cb->arg;
    continue;
    }

  /* The endpass "condition" just sets a flag to show it occurred. This is
  checked at compile time to be on an "accept" or "discard" item. */

  if (cb->type == ACLC_ENDPASS)
    {
    *epp = TRUE;
    continue;
    }

  /* For other conditions and modifiers, the argument is expanded now for some
  of them, but not for all, because expansion happens down in some lower level
  checking functions in some cases. */

  if (!conditions[cb->type].expand_at_top)
    arg = cb->arg;
  else if (!(arg = expand_string(cb->arg)))
    {
    if (f.expand_string_forcedfail) continue;
    *log_msgptr = string_sprintf("failed to expand ACL string \"%s\": %s",
      cb->arg, expand_string_message);
    return f.search_find_defer ? DEFER : ERROR;
    }

  /* Show condition, and expanded condition if it's different */

  HDEBUG(D_acl)
    {
    int lhswidth = 0;
    debug_printf_indent("check %s%s %n",
      (!conditions[cb->type].is_modifier && cb->u.negated)? "!":"",
      conditions[cb->type].name, &lhswidth);

    if (cb->type == ACLC_SET)
      {
#ifndef DISABLE_DKIM
      if (  Ustrcmp(cb->u.varname, "dkim_verify_status") == 0
	 || Ustrcmp(cb->u.varname, "dkim_verify_reason") == 0)
	{
	debug_printf("%s ", cb->u.varname);
	lhswidth += 19;
	}
      else
#endif
	{
	debug_printf("acl_%s ", cb->u.varname);
	lhswidth += 5 + Ustrlen(cb->u.varname);
	}
      }

    debug_printf("= %s\n", cb->arg);

    if (arg != cb->arg)
      debug_printf("%.*s= %s\n", lhswidth,
      US"                             ", CS arg);
    }

  /* Check that this condition makes sense at this time */

  if ((conditions[cb->type].forbids & (1 << where)) != 0)
    {
    *log_msgptr = string_sprintf("cannot %s %s condition in %s ACL",
      conditions[cb->type].is_modifier ? "use" : "test",
      conditions[cb->type].name, acl_wherenames[where]);
    return ERROR;
    }

  /* Run the appropriate test for each condition, or take the appropriate
  action for the remaining modifiers. */

  switch(cb->type)
    {
    case ACLC_ADD_HEADER:
    setup_header(arg);
    break;

    /* A nested ACL that returns "discard" makes sense only for an "accept" or
    "discard" verb. */

    case ACLC_ACL:
      rc = acl_check_wargs(where, addr, arg, user_msgptr, log_msgptr);
      if (rc == DISCARD && verb != ACL_ACCEPT && verb != ACL_DISCARD)
        {
        *log_msgptr = string_sprintf("nested ACL returned \"discard\" for "
          "\"%s\" command (only allowed with \"accept\" or \"discard\")",
          verbs[verb]);
        return ERROR;
        }
    break;

    case ACLC_AUTHENTICATED:
      rc = sender_host_authenticated ? match_isinlist(sender_host_authenticated,
	      &arg, 0, NULL, NULL, MCL_STRING, TRUE, NULL) : FAIL;
    break;

    #ifdef EXPERIMENTAL_BRIGHTMAIL
    case ACLC_BMI_OPTIN:
      {
      int old_pool = store_pool;
      store_pool = POOL_PERM;
      bmi_current_optin = string_copy(arg);
      store_pool = old_pool;
      }
    break;
    #endif

    case ACLC_CONDITION:
    /* The true/false parsing here should be kept in sync with that used in
    expand.c when dealing with ECOND_BOOL so that we don't have too many
    different definitions of what can be a boolean. */
    if (*arg == '-'
	? Ustrspn(arg+1, "0123456789") == Ustrlen(arg+1)    /* Negative number */
	: Ustrspn(arg,   "0123456789") == Ustrlen(arg))     /* Digits, or empty */
      rc = (Uatoi(arg) == 0)? FAIL : OK;
    else
      rc = (strcmpic(arg, US"no") == 0 ||
            strcmpic(arg, US"false") == 0)? FAIL :
           (strcmpic(arg, US"yes") == 0 ||
            strcmpic(arg, US"true") == 0)? OK : DEFER;
    if (rc == DEFER)
      *log_msgptr = string_sprintf("invalid \"condition\" value \"%s\"", arg);
    break;

    case ACLC_CONTINUE:    /* Always succeeds */
    break;

    case ACLC_CONTROL:
      {
      const uschar *p = NULL;
      control_type = decode_control(arg, &p, where, log_msgptr);

      /* Check if this control makes sense at this time */

      if (controls_list[control_type].forbids & (1 << where))
	{
	*log_msgptr = string_sprintf("cannot use \"control=%s\" in %s ACL",
	  controls_list[control_type].name, acl_wherenames[where]);
	return ERROR;
	}

      switch(control_type)
	{
	case CONTROL_AUTH_UNADVERTISED:
	f.allow_auth_unadvertised = TRUE;
	break;

	#ifdef EXPERIMENTAL_BRIGHTMAIL
	case CONTROL_BMI_RUN:
	bmi_run = 1;
	break;
	#endif

	#ifndef DISABLE_DKIM
	case CONTROL_DKIM_VERIFY:
	f.dkim_disable_verify = TRUE;
	#ifdef EXPERIMENTAL_DMARC
	/* Since DKIM was blocked, skip DMARC too */
	f.dmarc_disable_verify = TRUE;
	f.dmarc_enable_forensic = FALSE;
	#endif
	break;
	#endif

	#ifdef EXPERIMENTAL_DMARC
	case CONTROL_DMARC_VERIFY:
	f.dmarc_disable_verify = TRUE;
	break;

	case CONTROL_DMARC_FORENSIC:
	f.dmarc_enable_forensic = TRUE;
	break;
	#endif

	case CONTROL_DSCP:
	if (*p == '/')
	  {
	  int fd, af, level, optname, value;
	  /* If we are acting on stdin, the setsockopt may fail if stdin is not
	  a socket; we can accept that, we'll just debug-log failures anyway. */
	  fd = fileno(smtp_in);
	  af = ip_get_address_family(fd);
	  if (af < 0)
	    {
	    HDEBUG(D_acl)
	      debug_printf_indent("smtp input is probably not a socket [%s], not setting DSCP\n",
		  strerror(errno));
	    break;
	    }
	  if (dscp_lookup(p+1, af, &level, &optname, &value))
	    {
	    if (setsockopt(fd, level, optname, &value, sizeof(value)) < 0)
	      {
	      HDEBUG(D_acl) debug_printf_indent("failed to set input DSCP[%s]: %s\n",
		  p+1, strerror(errno));
	      }
	    else
	      {
	      HDEBUG(D_acl) debug_printf_indent("set input DSCP to \"%s\"\n", p+1);
	      }
	    }
	  else
	    {
	    *log_msgptr = string_sprintf("unrecognised DSCP value in \"control=%s\"", arg);
	    return ERROR;
	    }
	  }
	else
	  {
	  *log_msgptr = string_sprintf("syntax error in \"control=%s\"", arg);
	  return ERROR;
	  }
	break;

	case CONTROL_ERROR:
	return ERROR;

	case CONTROL_CASEFUL_LOCAL_PART:
	deliver_localpart = addr->cc_local_part;
	break;

	case CONTROL_CASELOWER_LOCAL_PART:
	deliver_localpart = addr->lc_local_part;
	break;

	case CONTROL_ENFORCE_SYNC:
	smtp_enforce_sync = TRUE;
	break;

	case CONTROL_NO_ENFORCE_SYNC:
	smtp_enforce_sync = FALSE;
	break;

	#ifdef WITH_CONTENT_SCAN
	case CONTROL_NO_MBOX_UNSPOOL:
	f.no_mbox_unspool = TRUE;
	break;
	#endif

	case CONTROL_NO_MULTILINE:
	f.no_multiline_responses = TRUE;
	break;

	case CONTROL_NO_PIPELINING:
	f.pipelining_enable = FALSE;
	break;

	case CONTROL_NO_DELAY_FLUSH:
	f.disable_delay_flush = TRUE;
	break;

	case CONTROL_NO_CALLOUT_FLUSH:
	f.disable_callout_flush = TRUE;
	break;

	case CONTROL_FAKEREJECT:
	cancel_cutthrough_connection(TRUE, US"fakereject");
	case CONTROL_FAKEDEFER:
	fake_response = (control_type == CONTROL_FAKEDEFER) ? DEFER : FAIL;
	if (*p == '/')
	  {
	  const uschar *pp = p + 1;
	  while (*pp != 0) pp++;
	  fake_response_text = expand_string(string_copyn(p+1, pp-p-1));
	  p = pp;
	  }
	 else
	  {
	  /* Explicitly reset to default string */
	  fake_response_text = US"Your message has been rejected but is being kept for evaluation.\nIf it was a legitimate message, it may still be delivered to the target recipient(s).";
	  }
	break;

	case CONTROL_FREEZE:
	f.deliver_freeze = TRUE;
	deliver_frozen_at = time(NULL);
	freeze_tell = freeze_tell_config;       /* Reset to configured value */
	if (Ustrncmp(p, "/no_tell", 8) == 0)
	  {
	  p += 8;
	  freeze_tell = NULL;
	  }
	if (*p != 0)
	  {
	  *log_msgptr = string_sprintf("syntax error in \"control=%s\"", arg);
	  return ERROR;
	  }
	cancel_cutthrough_connection(TRUE, US"item frozen");
	break;

	case CONTROL_QUEUE_ONLY:
	f.queue_only_policy = TRUE;
	cancel_cutthrough_connection(TRUE, US"queueing forced");
	break;

#if defined(SUPPORT_TLS) && defined(EXPERIMENTAL_REQUIRETLS)
	case CONTROL_REQUIRETLS:
	tls_requiretls |= REQUIRETLS_MSG;
	break;
#endif
	case CONTROL_SUBMISSION:
	originator_name = US"";
	f.submission_mode = TRUE;
	while (*p == '/')
	  {
	  if (Ustrncmp(p, "/sender_retain", 14) == 0)
	    {
	    p += 14;
	    f.active_local_sender_retain = TRUE;
	    f.active_local_from_check = FALSE;
	    }
	  else if (Ustrncmp(p, "/domain=", 8) == 0)
	    {
	    const uschar *pp = p + 8;
	    while (*pp != 0 && *pp != '/') pp++;
	    submission_domain = string_copyn(p+8, pp-p-8);
	    p = pp;
	    }
	  /* The name= option must be last, because it swallows the rest of
	  the string. */
	  else if (Ustrncmp(p, "/name=", 6) == 0)
	    {
	    const uschar *pp = p + 6;
	    while (*pp != 0) pp++;
	    submission_name = string_copy(parse_fix_phrase(p+6, pp-p-6,
	      big_buffer, big_buffer_size));
	    p = pp;
	    }
	  else break;
	  }
	if (*p != 0)
	  {
	  *log_msgptr = string_sprintf("syntax error in \"control=%s\"", arg);
	  return ERROR;
	  }
	break;

	case CONTROL_DEBUG:
	  {
	  uschar * debug_tag = NULL;
	  uschar * debug_opts = NULL;
	  BOOL kill = FALSE;

	  while (*p == '/')
	    {
	    const uschar * pp = p+1;
	    if (Ustrncmp(pp, "tag=", 4) == 0)
	      {
	      for (pp += 4; *pp && *pp != '/';) pp++;
	      debug_tag = string_copyn(p+5, pp-p-5);
	      }
	    else if (Ustrncmp(pp, "opts=", 5) == 0)
	      {
	      for (pp += 5; *pp && *pp != '/';) pp++;
	      debug_opts = string_copyn(p+6, pp-p-6);
	      }
	    else if (Ustrncmp(pp, "kill", 4) == 0)
	      {
	      for (pp += 4; *pp && *pp != '/';) pp++;
	      kill = TRUE;
	      }
	    else
	      while (*pp && *pp != '/') pp++;
	    p = pp;
	    }

	    if (kill)
	      debug_logging_stop();
	    else
	      debug_logging_activate(debug_tag, debug_opts);
	  }
	break;

	case CONTROL_SUPPRESS_LOCAL_FIXUPS:
	f.suppress_local_fixups = TRUE;
	break;

	case CONTROL_CUTTHROUGH_DELIVERY:
	{
	uschar * ignored = NULL;
#ifndef DISABLE_PRDR
	if (prdr_requested)
#else
	if (0)
#endif
	  /* Too hard to think about for now.  We might in future cutthrough
	  the case where both sides handle prdr and this-node prdr acl
	  is "accept" */
	  ignored = US"PRDR active";
	else
	  {
	  if (f.deliver_freeze)
	    ignored = US"frozen";
	  else if (f.queue_only_policy)
	    ignored = US"queue-only";
	  else if (fake_response == FAIL)
	    ignored = US"fakereject";
	  else
	    {
	    if (rcpt_count == 1)
	      {
	      cutthrough.delivery = TRUE;	/* control accepted */
	      while (*p == '/')
		{
		const uschar * pp = p+1;
		if (Ustrncmp(pp, "defer=", 6) == 0)
		  {
		  pp += 6;
		  if (Ustrncmp(pp, "pass", 4) == 0) cutthrough.defer_pass = TRUE;
		  /* else if (Ustrncmp(pp, "spool") == 0) ;	default */
		  }
		else
		  while (*pp && *pp != '/') pp++;
		p = pp;
		}
	      }
	    else
	      ignored = US"nonfirst rcpt";
	    }
	  }
	DEBUG(D_acl) if (ignored)
	  debug_printf(" cutthrough request ignored on %s item\n", ignored);
	}
	break;

#ifdef SUPPORT_I18N
	case CONTROL_UTF8_DOWNCONVERT:
	if (*p == '/')
	  {
	  if (p[1] == '1')
	    {
	    message_utf8_downconvert = 1;
	    addr->prop.utf8_downcvt = TRUE;
	    addr->prop.utf8_downcvt_maybe = FALSE;
	    p += 2;
	    break;
	    }
	  if (p[1] == '0')
	    {
	    message_utf8_downconvert = 0;
	    addr->prop.utf8_downcvt = FALSE;
	    addr->prop.utf8_downcvt_maybe = FALSE;
	    p += 2;
	    break;
	    }
	  if (p[1] == '-' && p[2] == '1')
	    {
	    message_utf8_downconvert = -1;
	    addr->prop.utf8_downcvt = FALSE;
	    addr->prop.utf8_downcvt_maybe = TRUE;
	    p += 3;
	    break;
	    }
	  *log_msgptr = US"bad option value for control=utf8_downconvert";
	  }
	else
	  {
	  message_utf8_downconvert = 1;
	  addr->prop.utf8_downcvt = TRUE;
	  addr->prop.utf8_downcvt_maybe = FALSE;
	  break;
	  }
	return ERROR;
#endif

	}
      break;
      }

    #ifdef EXPERIMENTAL_DCC
    case ACLC_DCC:
      {
      /* Separate the regular expression and any optional parameters. */
      const uschar * list = arg;
      uschar *ss = string_nextinlist(&list, &sep, big_buffer, big_buffer_size);
      /* Run the dcc backend. */
      rc = dcc_process(&ss);
      /* Modify return code based upon the existence of options. */
      while ((ss = string_nextinlist(&list, &sep, big_buffer, big_buffer_size)))
        if (strcmpic(ss, US"defer_ok") == 0 && rc == DEFER)
          rc = FAIL;   /* FAIL so that the message is passed to the next ACL */
      }
    break;
    #endif

    #ifdef WITH_CONTENT_SCAN
    case ACLC_DECODE:
    rc = mime_decode(&arg);
    break;
    #endif

    case ACLC_DELAY:
      {
      int delay = readconf_readtime(arg, 0, FALSE);
      if (delay < 0)
        {
        *log_msgptr = string_sprintf("syntax error in argument for \"delay\" "
          "modifier: \"%s\" is not a time value", arg);
        return ERROR;
        }
      else
        {
        HDEBUG(D_acl) debug_printf_indent("delay modifier requests %d-second delay\n",
          delay);
        if (host_checking)
          {
          HDEBUG(D_acl)
            debug_printf_indent("delay skipped in -bh checking mode\n");
          }

	/* NOTE 1: Remember that we may be
        dealing with stdin/stdout here, in addition to TCP/IP connections.
        Also, delays may be specified for non-SMTP input, where smtp_out and
        smtp_in will be NULL. Whatever is done must work in all cases.

        NOTE 2: The added feature of flushing the output before a delay must
        apply only to SMTP input. Hence the test for smtp_out being non-NULL.
        */

        else
          {
          if (smtp_out && !f.disable_delay_flush)
	    mac_smtp_fflush();

#if !defined(NO_POLL_H) && defined (POLLRDHUP)
	    {
	    struct pollfd p;
	    nfds_t n = 0;
	    if (smtp_out)
	      {
	      p.fd = fileno(smtp_out);
	      p.events = POLLRDHUP;
	      n = 1;
	      }
	    if (poll(&p, n, delay*1000) > 0)
	      HDEBUG(D_acl) debug_printf_indent("delay cancelled by peer close\n");
	    }
#else
	  /* Lacking POLLRDHUP it appears to be impossible to detect that a
	  TCP/IP connection has gone away without reading from it. This means
	  that we cannot shorten the delay below if the client goes away,
	  because we cannot discover that the client has closed its end of the
	  connection. (The connection is actually in a half-closed state,
	  waiting for the server to close its end.) It would be nice to be able
	  to detect this state, so that the Exim process is not held up
	  unnecessarily. However, it seems that we can't. The poll() function
	  does not do the right thing, and in any case it is not always
	  available.  */

          while (delay > 0) delay = sleep(delay);
#endif
          }
        }
      }
    break;

    #ifndef DISABLE_DKIM
    case ACLC_DKIM_SIGNER:
    if (dkim_cur_signer)
      rc = match_isinlist(dkim_cur_signer,
                          &arg,0,NULL,NULL,MCL_STRING,TRUE,NULL);
    else
      rc = FAIL;
    break;

    case ACLC_DKIM_STATUS:
    rc = match_isinlist(dkim_verify_status,
                        &arg,0,NULL,NULL,MCL_STRING,TRUE,NULL);
    break;
    #endif

    #ifdef EXPERIMENTAL_DMARC
    case ACLC_DMARC_STATUS:
    if (!f.dmarc_has_been_checked)
      dmarc_process();
    f.dmarc_has_been_checked = TRUE;
    /* used long way of dmarc_exim_expand_query() in case we need more
     * view into the process in the future. */
    rc = match_isinlist(dmarc_exim_expand_query(DMARC_VERIFY_STATUS),
                        &arg,0,NULL,NULL,MCL_STRING,TRUE,NULL);
    break;
    #endif

    case ACLC_DNSLISTS:
    rc = verify_check_dnsbl(where, &arg, log_msgptr);
    break;

    case ACLC_DOMAINS:
    rc = match_isinlist(addr->domain, &arg, 0, &domainlist_anchor,
      addr->domain_cache, MCL_DOMAIN, TRUE, CUSS &deliver_domain_data);
    break;

    /* The value in tls_cipher is the full cipher name, for example,
    TLSv1:DES-CBC3-SHA:168, whereas the values to test for are just the
    cipher names such as DES-CBC3-SHA. But program defensively. We don't know
    what may in practice come out of the SSL library - which at the time of
    writing is poorly documented. */

    case ACLC_ENCRYPTED:
    if (tls_in.cipher == NULL) rc = FAIL; else
      {
      uschar *endcipher = NULL;
      uschar *cipher = Ustrchr(tls_in.cipher, ':');
      if (cipher == NULL) cipher = tls_in.cipher; else
        {
        endcipher = Ustrchr(++cipher, ':');
        if (endcipher != NULL) *endcipher = 0;
        }
      rc = match_isinlist(cipher, &arg, 0, NULL, NULL, MCL_STRING, TRUE, NULL);
      if (endcipher != NULL) *endcipher = ':';
      }
    break;

    /* Use verify_check_this_host() instead of verify_check_host() so that
    we can pass over &host_data to catch any looked up data. Once it has been
    set, it retains its value so that it's still there if another ACL verb
    comes through here and uses the cache. However, we must put it into
    permanent store in case it is also expected to be used in a subsequent
    message in the same SMTP connection. */

    case ACLC_HOSTS:
    rc = verify_check_this_host(&arg, sender_host_cache, NULL,
      (sender_host_address == NULL)? US"" : sender_host_address,
      CUSS &host_data);
    if (rc == DEFER) *log_msgptr = search_error_message;
    if (host_data) host_data = string_copy_malloc(host_data);
    break;

    case ACLC_LOCAL_PARTS:
    rc = match_isinlist(addr->cc_local_part, &arg, 0,
      &localpartlist_anchor, addr->localpart_cache, MCL_LOCALPART, TRUE,
      CUSS &deliver_localpart_data);
    break;

    case ACLC_LOG_REJECT_TARGET:
      {
      int logbits = 0;
      int sep = 0;
      const uschar *s = arg;
      uschar *ss;
      while ((ss = string_nextinlist(&s, &sep, big_buffer, big_buffer_size)))
        {
        if (Ustrcmp(ss, "main") == 0) logbits |= LOG_MAIN;
        else if (Ustrcmp(ss, "panic") == 0) logbits |= LOG_PANIC;
        else if (Ustrcmp(ss, "reject") == 0) logbits |= LOG_REJECT;
        else
          {
          logbits |= LOG_MAIN|LOG_REJECT;
          log_write(0, LOG_MAIN|LOG_PANIC, "unknown log name \"%s\" in "
            "\"log_reject_target\" in %s ACL", ss, acl_wherenames[where]);
          }
        }
      log_reject_target = logbits;
      }
    break;

    case ACLC_LOGWRITE:
      {
      int logbits = 0;
      const uschar *s = arg;
      if (*s == ':')
        {
        s++;
        while (*s != ':')
          {
          if (Ustrncmp(s, "main", 4) == 0)
            { logbits |= LOG_MAIN; s += 4; }
          else if (Ustrncmp(s, "panic", 5) == 0)
            { logbits |= LOG_PANIC; s += 5; }
          else if (Ustrncmp(s, "reject", 6) == 0)
            { logbits |= LOG_REJECT; s += 6; }
          else
            {
            logbits = LOG_MAIN|LOG_PANIC;
            s = string_sprintf(":unknown log name in \"%s\" in "
              "\"logwrite\" in %s ACL", arg, acl_wherenames[where]);
            }
          if (*s == ',') s++;
          }
        s++;
        }
      while (isspace(*s)) s++;

      if (logbits == 0) logbits = LOG_MAIN;
      log_write(0, logbits, "%s", string_printing(s));
      }
    break;

    #ifdef WITH_CONTENT_SCAN
    case ACLC_MALWARE:			/* Run the malware backend. */
      {
      /* Separate the regular expression and any optional parameters. */
      const uschar * list = arg;
      uschar *ss = string_nextinlist(&list, &sep, big_buffer, big_buffer_size);
      uschar *opt;
      BOOL defer_ok = FALSE;
      int timeout = 0;

      while ((opt = string_nextinlist(&list, &sep, NULL, 0)))
        if (strcmpic(opt, US"defer_ok") == 0)
	  defer_ok = TRUE;
	else if (  strncmpic(opt, US"tmo=", 4) == 0
		&& (timeout = readconf_readtime(opt+4, '\0', FALSE)) < 0
		)
	  {
	  *log_msgptr = string_sprintf("bad timeout value in '%s'", opt);
	  return ERROR;
	  }

      rc = malware(ss, timeout);
      if (rc == DEFER && defer_ok)
	rc = FAIL;	/* FAIL so that the message is passed to the next ACL */
      }
    break;

    case ACLC_MIME_REGEX:
    rc = mime_regex(&arg);
    break;
    #endif

    case ACLC_QUEUE:
    if (Ustrchr(arg, '/'))
      {
      *log_msgptr = string_sprintf(
	      "Directory separator not permitted in queue name: '%s'", arg);
      return ERROR;
      }
    queue_name = string_copy_malloc(arg);
    break;

    case ACLC_RATELIMIT:
    rc = acl_ratelimit(arg, where, log_msgptr);
    break;

    case ACLC_RECIPIENTS:
    rc = match_address_list(CUS addr->address, TRUE, TRUE, &arg, NULL, -1, 0,
      CUSS &recipient_data);
    break;

    #ifdef WITH_CONTENT_SCAN
    case ACLC_REGEX:
    rc = regex(&arg);
    break;
    #endif

    case ACLC_REMOVE_HEADER:
    setup_remove_header(arg);
    break;

    case ACLC_SENDER_DOMAINS:
      {
      uschar *sdomain;
      sdomain = Ustrrchr(sender_address, '@');
      sdomain = sdomain ? sdomain + 1 : US"";
      rc = match_isinlist(sdomain, &arg, 0, &domainlist_anchor,
        sender_domain_cache, MCL_DOMAIN, TRUE, NULL);
      }
    break;

    case ACLC_SENDERS:
    rc = match_address_list(CUS sender_address, TRUE, TRUE, &arg,
      sender_address_cache, -1, 0, CUSS &sender_data);
    break;

    /* Connection variables must persist forever */

    case ACLC_SET:
      {
      int old_pool = store_pool;
      if (  cb->u.varname[0] == 'c'
#ifndef DISABLE_DKIM
         || cb->u.varname[0] == 'd'
#endif
#ifndef DISABLE_EVENT
	 || event_name		/* An event is being delivered */
#endif
	 )
        store_pool = POOL_PERM;
#ifndef DISABLE_DKIM	/* Overwriteable dkim result variables */
      if (Ustrcmp(cb->u.varname, "dkim_verify_status") == 0)
	dkim_verify_status = string_copy(arg);
      else if (Ustrcmp(cb->u.varname, "dkim_verify_reason") == 0)
	dkim_verify_reason = string_copy(arg);
      else
#endif
	acl_var_create(cb->u.varname)->data.ptr = string_copy(arg);
      store_pool = old_pool;
      }
    break;

#ifdef WITH_CONTENT_SCAN
    case ACLC_SPAM:
      {
      /* Separate the regular expression and any optional parameters. */
      const uschar * list = arg;
      uschar *ss = string_nextinlist(&list, &sep, big_buffer, big_buffer_size);

      rc = spam(CUSS &ss);
      /* Modify return code based upon the existence of options. */
      while ((ss = string_nextinlist(&list, &sep, big_buffer, big_buffer_size)))
        if (strcmpic(ss, US"defer_ok") == 0 && rc == DEFER)
          rc = FAIL;	/* FAIL so that the message is passed to the next ACL */
      }
    break;
#endif

#ifdef SUPPORT_SPF
    case ACLC_SPF:
      rc = spf_process(&arg, sender_address, SPF_PROCESS_NORMAL);
    break;
    case ACLC_SPF_GUESS:
      rc = spf_process(&arg, sender_address, SPF_PROCESS_GUESS);
    break;
#endif

    case ACLC_UDPSEND:
    rc = acl_udpsend(arg, log_msgptr);
    break;

    /* If the verb is WARN, discard any user message from verification, because
    such messages are SMTP responses, not header additions. The latter come
    only from explicit "message" modifiers. However, put the user message into
    $acl_verify_message so it can be used in subsequent conditions or modifiers
    (until something changes it). */

    case ACLC_VERIFY:
    rc = acl_verify(where, addr, arg, user_msgptr, log_msgptr, basic_errno);
    if (*user_msgptr)
      acl_verify_message = *user_msgptr;
    if (verb == ACL_WARN) *user_msgptr = NULL;
    break;

    default:
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "internal ACL error: unknown "
      "condition %d", cb->type);
    break;
    }

  /* If a condition was negated, invert OK/FAIL. */

  if (!conditions[cb->type].is_modifier && cb->u.negated)
    if (rc == OK) rc = FAIL;
    else if (rc == FAIL || rc == FAIL_DROP) rc = OK;

  if (rc != OK) break;   /* Conditions loop */
  }


/* If the result is the one for which "message" and/or "log_message" are used,
handle the values of these modifiers. If there isn't a log message set, we make
it the same as the user message.

"message" is a user message that will be included in an SMTP response. Unless
it is empty, it overrides any previously set user message.

"log_message" is a non-user message, and it adds to any existing non-user
message that is already set.

Most verbs have but a single return for which the messages are relevant, but
for "discard", it's useful to have the log message both when it succeeds and
when it fails. For "accept", the message is used in the OK case if there is no
"endpass", but (for backwards compatibility) in the FAIL case if "endpass" is
present. */

if (*epp && rc == OK) user_message = NULL;

if ((BIT(rc) & msgcond[verb]) != 0)
  {
  uschar *expmessage;
  uschar *old_user_msgptr = *user_msgptr;
  uschar *old_log_msgptr = (*log_msgptr != NULL)? *log_msgptr : old_user_msgptr;

  /* If the verb is "warn", messages generated by conditions (verification or
  nested ACLs) are always discarded. This also happens for acceptance verbs
  when they actually do accept. Only messages specified at this level are used.
  However, the value of an existing message is available in $acl_verify_message
  during expansions. */

  if (verb == ACL_WARN ||
      (rc == OK && (verb == ACL_ACCEPT || verb == ACL_DISCARD)))
    *log_msgptr = *user_msgptr = NULL;

  if (user_message)
    {
    acl_verify_message = old_user_msgptr;
    expmessage = expand_string(user_message);
    if (!expmessage)
      {
      if (!f.expand_string_forcedfail)
        log_write(0, LOG_MAIN|LOG_PANIC, "failed to expand ACL message \"%s\": %s",
          user_message, expand_string_message);
      }
    else if (expmessage[0] != 0) *user_msgptr = expmessage;
    }

  if (log_message)
    {
    acl_verify_message = old_log_msgptr;
    expmessage = expand_string(log_message);
    if (!expmessage)
      {
      if (!f.expand_string_forcedfail)
        log_write(0, LOG_MAIN|LOG_PANIC, "failed to expand ACL message \"%s\": %s",
          log_message, expand_string_message);
      }
    else if (expmessage[0] != 0)
      {
      *log_msgptr = (*log_msgptr == NULL)? expmessage :
        string_sprintf("%s: %s", expmessage, *log_msgptr);
      }
    }

  /* If no log message, default it to the user message */

  if (!*log_msgptr) *log_msgptr = *user_msgptr;
  }

acl_verify_message = NULL;
return rc;
}