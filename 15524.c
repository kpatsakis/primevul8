peer_lookup_vty (struct vty *vty, const char *ip_str)
{
  int ret;
  struct bgp *bgp;
  union sockunion su;
  struct peer *peer;

  bgp = vty->index;

  ret = str2sockunion (ip_str, &su);
  if (ret < 0)
    {
      vty_out (vty, "%% Malformed address: %s%s", ip_str, VTY_NEWLINE);
      return NULL;
    }

  peer = peer_lookup (bgp, &su);
  if (! peer)
    {
      vty_out (vty, "%% Specify remote-as or peer-group commands first%s", VTY_NEWLINE);
      return NULL;
    }
  return peer;
}