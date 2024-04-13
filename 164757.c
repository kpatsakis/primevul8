my_exit_policy_rejects(const tor_addr_t *addr,
                       uint16_t port,
                       const char **why_rejected)
{
  if (router_compare_to_my_exit_policy(addr, port)) {
    *why_rejected = "";
    return 1;
  } else if (tor_addr_family(addr) == AF_INET6 && !get_options()->IPv6Exit) {
    *why_rejected = " (IPv6 address without IPv6Exit configured)";
    return 1;
  }
  return 0;
}