acl_verify_csa(const uschar *domain)
{
tree_node *t;
const uschar *found;
int priority, weight, port;
dns_answer dnsa;
dns_scan dnss;
dns_record *rr;
int rc, type;
uschar target[256];

/* Work out the domain we are using for the CSA lookup. The default is the
client's HELO domain. If the client has not said HELO, use its IP address
instead. If it's a local client (exim -bs), CSA isn't applicable. */

while (isspace(*domain) && *domain != '\0') ++domain;
if (*domain == '\0') domain = sender_helo_name;
if (domain == NULL) domain = sender_host_address;
if (sender_host_address == NULL) return CSA_UNKNOWN;

/* If we have an address literal, strip off the framing ready for turning it
into a domain. The framing consists of matched square brackets possibly
containing a keyword and a colon before the actual IP address. */

if (domain[0] == '[')
  {
  const uschar *start = Ustrchr(domain, ':');
  if (start == NULL) start = domain;
  domain = string_copyn(start + 1, Ustrlen(start) - 2);
  }

/* Turn domains that look like bare IP addresses into domains in the reverse
DNS. This code also deals with address literals and $sender_host_address. It's
not quite kosher to treat bare domains such as EHLO 192.0.2.57 the same as
address literals, but it's probably the most friendly thing to do. This is an
extension to CSA, so we allow it to be turned off for proper conformance. */

if (string_is_ip_address(domain, NULL) != 0)
  {
  if (!dns_csa_use_reverse) return CSA_UNKNOWN;
  dns_build_reverse(domain, target);
  domain = target;
  }

/* Find out if we've already done the CSA check for this domain. If we have,
return the same result again. Otherwise build a new cached result structure
for this domain. The name is filled in now, and the value is filled in when
we return from this function. */

t = tree_search(csa_cache, domain);
if (t != NULL) return t->data.val;

t = store_get_perm(sizeof(tree_node) + Ustrlen(domain));
Ustrcpy(t->name, domain);
(void)tree_insertnode(&csa_cache, t);

/* Now we are ready to do the actual DNS lookup(s). */

found = domain;
switch (dns_special_lookup(&dnsa, domain, T_CSA, &found))
  {
  /* If something bad happened (most commonly DNS_AGAIN), defer. */

  default:
  return t->data.val = CSA_DEFER_SRV;

  /* If we found nothing, the client's authorization is unknown. */

  case DNS_NOMATCH:
  case DNS_NODATA:
  return t->data.val = CSA_UNKNOWN;

  /* We got something! Go on to look at the reply in more detail. */

  case DNS_SUCCEED:
  break;
  }

/* Scan the reply for well-formed CSA SRV records. */

for (rr = dns_next_rr(&dnsa, &dnss, RESET_ANSWERS);
     rr;
     rr = dns_next_rr(&dnsa, &dnss, RESET_NEXT)) if (rr->type == T_SRV)
  {
  const uschar * p = rr->data;

  /* Extract the numerical SRV fields (p is incremented) */

  GETSHORT(priority, p);
  GETSHORT(weight, p);
  GETSHORT(port, p);

  DEBUG(D_acl)
    debug_printf_indent("CSA priority=%d weight=%d port=%d\n", priority, weight, port);

  /* Check the CSA version number */

  if (priority != 1) continue;

  /* If the domain does not have a CSA SRV record of its own (i.e. the domain
  found by dns_special_lookup() is a parent of the one we asked for), we check
  the subdomain assertions in the port field. At the moment there's only one
  assertion: legitimate SMTP clients are all explicitly authorized with CSA
  SRV records of their own. */

  if (Ustrcmp(found, domain) != 0)
    return t->data.val = port & 1 ? CSA_FAIL_EXPLICIT : CSA_UNKNOWN;

  /* This CSA SRV record refers directly to our domain, so we check the value
  in the weight field to work out the domain's authorization. 0 and 1 are
  unauthorized; 3 means the client is authorized but we can't check the IP
  address in order to authenticate it, so we treat it as unknown; values
  greater than 3 are undefined. */

  if (weight < 2) return t->data.val = CSA_FAIL_DOMAIN;

  if (weight > 2) continue;

  /* Weight == 2, which means the domain is authorized. We must check that the
  client's IP address is listed as one of the SRV target addresses. Save the
  target hostname then break to scan the additional data for its addresses. */

  (void)dn_expand(dnsa.answer, dnsa.answer + dnsa.answerlen, p,
    (DN_EXPAND_ARG4_TYPE)target, sizeof(target));

  DEBUG(D_acl) debug_printf_indent("CSA target is %s\n", target);

  break;
  }

/* If we didn't break the loop then no appropriate records were found. */

if (rr == NULL) return t->data.val = CSA_UNKNOWN;

/* Do not check addresses if the target is ".", in accordance with RFC 2782.
A target of "." indicates there are no valid addresses, so the client cannot
be authorized. (This is an odd configuration because weight=2 target=. is
equivalent to weight=1, but we check for it in order to keep load off the
root name servers.) Note that dn_expand() turns "." into "". */

if (Ustrcmp(target, "") == 0) return t->data.val = CSA_FAIL_NOADDR;

/* Scan the additional section of the CSA SRV reply for addresses belonging
to the target. If the name server didn't return any additional data (e.g.
because it does not fully support SRV records), we need to do another lookup
to obtain the target addresses; otherwise we have a definitive result. */

rc = acl_verify_csa_address(&dnsa, &dnss, RESET_ADDITIONAL, target);
if (rc != CSA_FAIL_NOADDR) return t->data.val = rc;

/* The DNS lookup type corresponds to the IP version used by the client. */

#if HAVE_IPV6
if (Ustrchr(sender_host_address, ':') != NULL)
  type = T_AAAA;
else
#endif /* HAVE_IPV6 */
  type = T_A;


lookup_dnssec_authenticated = NULL;
switch (dns_lookup(&dnsa, target, type, NULL))
  {
  /* If something bad happened (most commonly DNS_AGAIN), defer. */

  default:
    return t->data.val = CSA_DEFER_ADDR;

  /* If the query succeeded, scan the addresses and return the result. */

  case DNS_SUCCEED:
    rc = acl_verify_csa_address(&dnsa, &dnss, RESET_ANSWERS, target);
    if (rc != CSA_FAIL_NOADDR) return t->data.val = rc;
    /* else fall through */

  /* If the target has no IP addresses, the client cannot have an authorized
  IP address. However, if the target site uses A6 records (not AAAA records)
  we have to do yet another lookup in order to check them. */

  case DNS_NOMATCH:
  case DNS_NODATA:
    return t->data.val = CSA_FAIL_NOADDR;
  }
}