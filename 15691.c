peer_weight_set_vty (struct vty *vty, const char *ip_str, 
                     const char *weight_str)
{
  int ret;
  struct peer *peer;
  unsigned long weight;

  peer = peer_and_group_lookup_vty (vty, ip_str);
  if (! peer)
    return CMD_WARNING;

  VTY_GET_INTEGER_RANGE("weight", weight, weight_str, 0, 65535);

  ret = peer_weight_set (peer, weight);

  return CMD_SUCCESS;
}