transport_rcpt_address(address_item *addr, BOOL include_affixes)
{
uschar *at;
int plen, slen;

if (include_affixes)
  {
  setflag(addr, af_include_affixes);  /* Affects logged => line */
  return addr->address;
  }

if (addr->suffix == NULL)
  {
  if (addr->prefix == NULL) return addr->address;
  return addr->address + Ustrlen(addr->prefix);
  }

at = Ustrrchr(addr->address, '@');
plen = (addr->prefix == NULL)? 0 : Ustrlen(addr->prefix);
slen = Ustrlen(addr->suffix);

return string_sprintf("%.*s@%s", (int)(at - addr->address - plen - slen),
   addr->address + plen, at + 1);
}