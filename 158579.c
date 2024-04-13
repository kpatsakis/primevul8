write_env_to(address_item *p, struct aci **pplist, struct aci **pdlist,
  BOOL *first, transport_ctx * tctx)
{
address_item *pp;
struct aci *ppp;

/* Do nothing if we have already handled this address. If not, remember it
so that we don't handle it again. */

for (ppp = *pdlist; ppp; ppp = ppp->next) if (p == ppp->ptr) return TRUE;

ppp = store_get(sizeof(struct aci));
ppp->next = *pdlist;
*pdlist = ppp;
ppp->ptr = p;

/* Now scan up the ancestry, checking for duplicates at each generation. */

for (pp = p;; pp = pp->parent)
  {
  address_item *dup;
  for (dup = addr_duplicate; dup; dup = dup->next)
    if (dup->dupof == pp)   /* a dup of our address */
      if (!write_env_to(dup, pplist, pdlist, first, tctx))
	return FALSE;
  if (!pp->parent) break;
  }

/* Check to see if we have already output the progenitor. */

for (ppp = *pplist; ppp; ppp = ppp->next) if (pp == ppp->ptr) break;
if (ppp) return TRUE;

/* Remember what we have output, and output it. */

ppp = store_get(sizeof(struct aci));
ppp->next = *pplist;
*pplist = ppp;
ppp->ptr = pp;

if (!*first && !write_chunk(tctx, US",\n ", 3)) return FALSE;
*first = FALSE;
return write_chunk(tctx, pp->address, Ustrlen(pp->address));
}