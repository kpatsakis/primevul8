peer_interface_vty (struct vty *vty, const char *ip_str, const char *str)
{
  int ret;
  struct peer *peer;

  peer = peer_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  if (str)
    ret = peer_interface_set (peer, str);
  else
    ret = peer_interface_unset (peer);

  return CMD_SUCCESS;
}