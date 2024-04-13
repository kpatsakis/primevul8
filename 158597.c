findugid(address_item *addr, transport_instance *tp, uid_t *uidp, gid_t *gidp,
  BOOL *igfp)
{
uschar *nuname;
BOOL gid_set = FALSE;

/* Default initgroups flag comes from the transport */

*igfp = tp->initgroups;

/* First see if there's a gid on the transport, either fixed or expandable.
The expanding function always logs failure itself. */

if (tp->gid_set)
  {
  *gidp = tp->gid;
  gid_set = TRUE;
  }
else if (tp->expand_gid)
  {
  if (!route_find_expanded_group(tp->expand_gid, tp->name, US"transport", gidp,
    &(addr->message)))
    {
    common_error(FALSE, addr, ERRNO_GIDFAIL, NULL);
    return FALSE;
    }
  gid_set = TRUE;
  }

/* If the transport did not set a group, see if the router did. */

if (!gid_set && testflag(addr, af_gid_set))
  {
  *gidp = addr->gid;
  gid_set = TRUE;
  }

/* Pick up a uid from the transport if one is set. */

if (tp->uid_set) *uidp = tp->uid;

/* Otherwise, try for an expandable uid field. If it ends up as a numeric id,
it does not provide a passwd value from which a gid can be taken. */

else if (tp->expand_uid)
  {
  struct passwd *pw;
  if (!route_find_expanded_user(tp->expand_uid, tp->name, US"transport", &pw,
       uidp, &(addr->message)))
    {
    common_error(FALSE, addr, ERRNO_UIDFAIL, NULL);
    return FALSE;
    }
  if (!gid_set && pw)
    {
    *gidp = pw->pw_gid;
    gid_set = TRUE;
    }
  }

/* If the transport doesn't set the uid, test the deliver_as_creator flag. */

else if (tp->deliver_as_creator)
  {
  *uidp = originator_uid;
  if (!gid_set)
    {
    *gidp = originator_gid;
    gid_set = TRUE;
    }
  }

/* Otherwise see if the address specifies the uid and if so, take it and its
initgroups flag. */

else if (testflag(addr, af_uid_set))
  {
  *uidp = addr->uid;
  *igfp = testflag(addr, af_initgroups);
  }

/* Nothing has specified the uid - default to the Exim user, and group if the
gid is not set. */

else
  {
  *uidp = exim_uid;
  if (!gid_set)
    {
    *gidp = exim_gid;
    gid_set = TRUE;
    }
  }

/* If no gid is set, it is a disaster. We default to the Exim gid only if
defaulting to the Exim uid. In other words, if the configuration has specified
a uid, it must also provide a gid. */

if (!gid_set)
  {
  common_error(TRUE, addr, ERRNO_GIDFAIL, US"User set without group for "
    "%s transport", tp->name);
  return FALSE;
  }

/* Check that the uid is not on the lists of banned uids that may not be used
for delivery processes. */

nuname = check_never_users(*uidp, never_users)
  ? US"never_users"
  : check_never_users(*uidp, fixed_never_users)
  ? US"fixed_never_users"
  : NULL;
if (nuname)
  {
  common_error(TRUE, addr, ERRNO_UIDFAIL, US"User %ld set for %s transport "
    "is on the %s list", (long int)(*uidp), tp->name, nuname);
  return FALSE;
  }

/* All is well */

return TRUE;
}