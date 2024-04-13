acl_check_internal(int where, address_item *addr, uschar *s,
  uschar **user_msgptr, uschar **log_msgptr)
{
int fd = -1;
acl_block *acl = NULL;
uschar *acl_name = US"inline ACL";
uschar *ss;

/* Catch configuration loops */

if (acl_level > 20)
  {
  *log_msgptr = US"ACL nested too deep: possible loop";
  return ERROR;
  }

if (!s)
  {
  HDEBUG(D_acl) debug_printf_indent("ACL is NULL: implicit DENY\n");
  return FAIL;
  }

/* At top level, we expand the incoming string. At lower levels, it has already
been expanded as part of condition processing. */

if (acl_level == 0)
  {
  if (!(ss = expand_string(s)))
    {
    if (f.expand_string_forcedfail) return OK;
    *log_msgptr = string_sprintf("failed to expand ACL string \"%s\": %s", s,
      expand_string_message);
    return ERROR;
    }
  }
else ss = s;

while (isspace(*ss)) ss++;

/* If we can't find a named ACL, the default is to parse it as an inline one.
(Unless it begins with a slash; non-existent files give rise to an error.) */

acl_text = ss;

/* Handle the case of a string that does not contain any spaces. Look for a
named ACL among those read from the configuration, or a previously read file.
It is possible that the pointer to the ACL is NULL if the configuration
contains a name with no data. If not found, and the text begins with '/',
read an ACL from a file, and save it so it can be re-used. */

if (Ustrchr(ss, ' ') == NULL)
  {
  tree_node *t = tree_search(acl_anchor, ss);
  if (t != NULL)
    {
    acl = (acl_block *)(t->data.ptr);
    if (acl == NULL)
      {
      HDEBUG(D_acl) debug_printf_indent("ACL \"%s\" is empty: implicit DENY\n", ss);
      return FAIL;
      }
    acl_name = string_sprintf("ACL \"%s\"", ss);
    HDEBUG(D_acl) debug_printf_indent("using ACL \"%s\"\n", ss);
    }

  else if (*ss == '/')
    {
    struct stat statbuf;
    fd = Uopen(ss, O_RDONLY, 0);
    if (fd < 0)
      {
      *log_msgptr = string_sprintf("failed to open ACL file \"%s\": %s", ss,
        strerror(errno));
      return ERROR;
      }

    if (fstat(fd, &statbuf) != 0)
      {
      *log_msgptr = string_sprintf("failed to fstat ACL file \"%s\": %s", ss,
        strerror(errno));
      return ERROR;
      }

    acl_text = store_get(statbuf.st_size + 1);
    acl_text_end = acl_text + statbuf.st_size + 1;

    if (read(fd, acl_text, statbuf.st_size) != statbuf.st_size)
      {
      *log_msgptr = string_sprintf("failed to read ACL file \"%s\": %s",
        ss, strerror(errno));
      return ERROR;
      }
    acl_text[statbuf.st_size] = 0;
    (void)close(fd);

    acl_name = string_sprintf("ACL \"%s\"", ss);
    HDEBUG(D_acl) debug_printf_indent("read ACL from file %s\n", ss);
    }
  }

/* Parse an ACL that is still in text form. If it came from a file, remember it
in the ACL tree, having read it into the POOL_PERM store pool so that it
persists between multiple messages. */

if (acl == NULL)
  {
  int old_pool = store_pool;
  if (fd >= 0) store_pool = POOL_PERM;
  acl = acl_read(acl_getline, log_msgptr);
  store_pool = old_pool;
  if (acl == NULL && *log_msgptr != NULL) return ERROR;
  if (fd >= 0)
    {
    tree_node *t = store_get_perm(sizeof(tree_node) + Ustrlen(ss));
    Ustrcpy(t->name, ss);
    t->data.ptr = acl;
    (void)tree_insertnode(&acl_anchor, t);
    }
  }

/* Now we have an ACL to use. It's possible it may be NULL. */

while (acl != NULL)
  {
  int cond;
  int basic_errno = 0;
  BOOL endpass_seen = FALSE;
  BOOL acl_quit_check = acl_level == 0
    && (where == ACL_WHERE_QUIT || where == ACL_WHERE_NOTQUIT);

  *log_msgptr = *user_msgptr = NULL;
  f.acl_temp_details = FALSE;

  HDEBUG(D_acl) debug_printf_indent("processing \"%s\"\n", verbs[acl->verb]);

  /* Clear out any search error message from a previous check before testing
  this condition. */

  search_error_message = NULL;
  cond = acl_check_condition(acl->verb, acl->condition, where, addr, acl_level,
    &endpass_seen, user_msgptr, log_msgptr, &basic_errno);

  /* Handle special returns: DEFER causes a return except on a WARN verb;
  ERROR always causes a return. */

  switch (cond)
    {
    case DEFER:
    HDEBUG(D_acl) debug_printf_indent("%s: condition test deferred in %s\n", verbs[acl->verb], acl_name);
    if (basic_errno != ERRNO_CALLOUTDEFER)
      {
      if (search_error_message != NULL && *search_error_message != 0)
        *log_msgptr = search_error_message;
      if (smtp_return_error_details) f.acl_temp_details = TRUE;
      }
    else
      f.acl_temp_details = TRUE;
    if (acl->verb != ACL_WARN) return DEFER;
    break;

    default:      /* Paranoia */
    case ERROR:
    HDEBUG(D_acl) debug_printf_indent("%s: condition test error in %s\n", verbs[acl->verb], acl_name);
    return ERROR;

    case OK:
    HDEBUG(D_acl) debug_printf_indent("%s: condition test succeeded in %s\n",
      verbs[acl->verb], acl_name);
    break;

    case FAIL:
    HDEBUG(D_acl) debug_printf_indent("%s: condition test failed in %s\n", verbs[acl->verb], acl_name);
    break;

    /* DISCARD and DROP can happen only from a nested ACL condition, and
    DISCARD can happen only for an "accept" or "discard" verb. */

    case DISCARD:
    HDEBUG(D_acl) debug_printf_indent("%s: condition test yielded \"discard\" in %s\n",
      verbs[acl->verb], acl_name);
    break;

    case FAIL_DROP:
    HDEBUG(D_acl) debug_printf_indent("%s: condition test yielded \"drop\" in %s\n",
      verbs[acl->verb], acl_name);
    break;
    }

  /* At this point, cond for most verbs is either OK or FAIL or (as a result of
  a nested ACL condition) FAIL_DROP. However, for WARN, cond may be DEFER, and
  for ACCEPT and DISCARD, it may be DISCARD after a nested ACL call. */

  switch(acl->verb)
    {
    case ACL_ACCEPT:
    if (cond == OK || cond == DISCARD)
      {
      HDEBUG(D_acl) debug_printf_indent("end of %s: ACCEPT\n", acl_name);
      return cond;
      }
    if (endpass_seen)
      {
      HDEBUG(D_acl) debug_printf_indent("accept: endpass encountered - denying access\n");
      return cond;
      }
    break;

    case ACL_DEFER:
    if (cond == OK)
      {
      HDEBUG(D_acl) debug_printf_indent("end of %s: DEFER\n", acl_name);
      if (acl_quit_check) goto badquit;
      f.acl_temp_details = TRUE;
      return DEFER;
      }
    break;

    case ACL_DENY:
    if (cond == OK)
      {
      HDEBUG(D_acl) debug_printf_indent("end of %s: DENY\n", acl_name);
      if (acl_quit_check) goto badquit;
      return FAIL;
      }
    break;

    case ACL_DISCARD:
    if (cond == OK || cond == DISCARD)
      {
      HDEBUG(D_acl) debug_printf_indent("end of %s: DISCARD\n", acl_name);
      if (acl_quit_check) goto badquit;
      return DISCARD;
      }
    if (endpass_seen)
      {
      HDEBUG(D_acl) debug_printf_indent("discard: endpass encountered - denying access\n");
      return cond;
      }
    break;

    case ACL_DROP:
    if (cond == OK)
      {
      HDEBUG(D_acl) debug_printf_indent("end of %s: DROP\n", acl_name);
      if (acl_quit_check) goto badquit;
      return FAIL_DROP;
      }
    break;

    case ACL_REQUIRE:
    if (cond != OK)
      {
      HDEBUG(D_acl) debug_printf_indent("end of %s: not OK\n", acl_name);
      if (acl_quit_check) goto badquit;
      return cond;
      }
    break;

    case ACL_WARN:
    if (cond == OK)
      acl_warn(where, *user_msgptr, *log_msgptr);
    else if (cond == DEFER && LOGGING(acl_warn_skipped))
      log_write(0, LOG_MAIN, "%s Warning: ACL \"warn\" statement skipped: "
        "condition test deferred%s%s", host_and_ident(TRUE),
        (*log_msgptr == NULL)? US"" : US": ",
        (*log_msgptr == NULL)? US"" : *log_msgptr);
    *log_msgptr = *user_msgptr = NULL;  /* In case implicit DENY follows */
    break;

    default:
    log_write(0, LOG_MAIN|LOG_PANIC_DIE, "internal ACL error: unknown verb %d",
      acl->verb);
    break;
    }

  /* Pass to the next ACL item */

  acl = acl->next;
  }

/* We have reached the end of the ACL. This is an implicit DENY. */

HDEBUG(D_acl) debug_printf_indent("end of %s: implicit DENY\n", acl_name);
return FAIL;

badquit:
  *log_msgptr = string_sprintf("QUIT or not-QUIT toplevel ACL may not fail "
    "('%s' verb used incorrectly)", verbs[acl->verb]);
  return ERROR;
}