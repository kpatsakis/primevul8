address_done(address_item *addr, uschar *now)
{
address_item *dup;

update_spool = TRUE;        /* Ensure spool gets updated */

/* Top-level address */

if (!addr->parent)
  {
  tree_add_nonrecipient(addr->unique);
  tree_add_nonrecipient(addr->address);
  }

/* Homonymous child address */

else if (testflag(addr, af_homonym))
  {
  if (addr->transport)
    tree_add_nonrecipient(
      string_sprintf("%s/%s", addr->unique + 3, addr->transport->name));
  }

/* Non-homonymous child address */

else tree_add_nonrecipient(addr->unique);

/* Check the list of duplicate addresses and ensure they are now marked
done as well. */

for (dup = addr_duplicate; dup; dup = dup->next)
  if (Ustrcmp(addr->unique, dup->unique) == 0)
    {
    tree_add_nonrecipient(dup->unique);
    child_done(dup, now);
    }
}