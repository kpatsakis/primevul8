string_log_address(gstring * g,
  address_item *addr, BOOL all_parents, BOOL success)
{
BOOL add_topaddr = TRUE;
address_item *topaddr;

/* Find the ultimate parent */

for (topaddr = addr; topaddr->parent; topaddr = topaddr->parent) ;

/* We start with just the local part for pipe, file, and reply deliveries, and
for successful local deliveries from routers that have the log_as_local flag
set. File deliveries from filters can be specified as non-absolute paths in
cases where the transport is going to complete the path. If there is an error
before this happens (expansion failure) the local part will not be updated, and
so won't necessarily look like a path. Add extra text for this case. */

if (  testflag(addr, af_pfr)
   || (  success
      && addr->router && addr->router->log_as_local
      && addr->transport && addr->transport->info->local
   )  )
  {
  if (testflag(addr, af_file) && addr->local_part[0] != '/')
    g = string_catn(g, CUS"save ", 5);
  g = string_get_localpart(addr, g);
  }

/* Other deliveries start with the full address. It we have split it into local
part and domain, use those fields. Some early failures can happen before the
splitting is done; in those cases use the original field. */

else
  {
  uschar * cmp = g->s + g->ptr;

  if (addr->local_part)
    {
    const uschar * s;
    g = string_get_localpart(addr, g);
    g = string_catn(g, US"@", 1);
    s = addr->domain;
#ifdef SUPPORT_I18N
    if (testflag(addr, af_utf8_downcvt))
      s = string_localpart_utf8_to_alabel(s, NULL);
#endif
    g = string_cat(g, s);
    }
  else
    g = string_cat(g, addr->address);

  /* If the address we are going to print is the same as the top address,
  and all parents are not being included, don't add on the top address. First
  of all, do a caseless comparison; if this succeeds, do a caseful comparison
  on the local parts. */

  string_from_gstring(g);	/* ensure nul-terminated */
  if (  strcmpic(cmp, topaddr->address) == 0
     && Ustrncmp(cmp, topaddr->address, Ustrchr(cmp, '@') - cmp) == 0
     && !addr->onetime_parent
     && (!all_parents || !addr->parent || addr->parent == topaddr)
     )
    add_topaddr = FALSE;
  }

/* If all parents are requested, or this is a local pipe/file/reply, and
there is at least one intermediate parent, show it in brackets, and continue
with all of them if all are wanted. */

if (  (all_parents || testflag(addr, af_pfr))
   && addr->parent
   && addr->parent != topaddr)
  {
  uschar *s = US" (";
  address_item *addr2;
  for (addr2 = addr->parent; addr2 != topaddr; addr2 = addr2->parent)
    {
    g = string_catn(g, s, 2);
    g = string_cat (g, addr2->address);
    if (!all_parents) break;
    s = US", ";
    }
  g = string_catn(g, US")", 1);
  }

/* Add the top address if it is required */

if (add_topaddr)
  g = string_append(g, 3,
    US" <",
    addr->onetime_parent ? addr->onetime_parent : topaddr->address,
    US">");

return g;
}