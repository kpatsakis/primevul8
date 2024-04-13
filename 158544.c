acl_verify_csa_address(dns_answer *dnsa, dns_scan *dnss, int reset,
                       uschar *target)
{
dns_record *rr;
dns_address *da;

BOOL target_found = FALSE;

for (rr = dns_next_rr(dnsa, dnss, reset);
     rr != NULL;
     rr = dns_next_rr(dnsa, dnss, RESET_NEXT))
  {
  /* Check this is an address RR for the target hostname. */

  if (rr->type != T_A
    #if HAVE_IPV6
      && rr->type != T_AAAA
    #endif
  ) continue;

  if (strcmpic(target, rr->name) != 0) continue;

  target_found = TRUE;

  /* Turn the target address RR into a list of textual IP addresses and scan
  the list. There may be more than one if it is an A6 RR. */

  for (da = dns_address_from_rr(dnsa, rr); da != NULL; da = da->next)
    {
    /* If the client IP address matches the target IP address, it's good! */

    DEBUG(D_acl) debug_printf_indent("CSA target address is %s\n", da->address);

    if (strcmpic(sender_host_address, da->address) == 0) return CSA_OK;
    }
  }

/* If we found some target addresses but none of them matched, the client is
using an unauthorized IP address, otherwise the target has no authorized IP
addresses. */

if (target_found) return CSA_FAIL_MISMATCH;
else return CSA_FAIL_NOADDR;
}