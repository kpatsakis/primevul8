clip_dns_ttl(uint32_t ttl)
{
  /* This logic is a defense against "DefectTor" DNS-based traffic
   * confirmation attacks, as in https://nymity.ch/tor-dns/tor-dns.pdf .
   * We only give two values: a "low" value and a "high" value.
   */
  if (ttl < MIN_DNS_TTL)
    return MIN_DNS_TTL;
  else
    return MAX_DNS_TTL;
}