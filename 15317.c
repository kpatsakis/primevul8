peer_address_self_check (union sockunion *su)
{
  struct interface *ifp = NULL;

  if (su->sa.sa_family == AF_INET)
    ifp = if_lookup_by_ipv4_exact (&su->sin.sin_addr);
#ifdef HAVE_IPV6
  else if (su->sa.sa_family == AF_INET6)
    ifp = if_lookup_by_ipv6_exact (&su->sin6.sin6_addr);
#endif /* HAVE IPV6 */

  if (ifp)
    return 1;

  return 0;
}