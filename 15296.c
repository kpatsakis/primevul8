bgp_vty_return (struct vty *vty, int ret)
{
  const char *str = NULL;

  switch (ret)
    {
    case BGP_ERR_INVALID_VALUE:
      str = "Invalid value";
      break;
    case BGP_ERR_INVALID_FLAG:
      str = "Invalid flag";
      break;
    case BGP_ERR_PEER_INACTIVE:
      str = "Activate the neighbor for the address family first";
      break;
    case BGP_ERR_INVALID_FOR_PEER_GROUP_MEMBER:
      str = "Invalid command for a peer-group member";
      break;
    case BGP_ERR_PEER_GROUP_SHUTDOWN:
      str = "Peer-group has been shutdown. Activate the peer-group first";
      break;
    case BGP_ERR_PEER_GROUP_HAS_THE_FLAG:
      str = "This peer is a peer-group member.  Please change peer-group configuration";
      break;
    case BGP_ERR_PEER_FLAG_CONFLICT:
      str = "Can't set override-capability and strict-capability-match at the same time";
      break;
    case BGP_ERR_PEER_GROUP_MEMBER_EXISTS:
      str = "No activate for peergroup can be given only if peer-group has no members";
      break;
    case BGP_ERR_PEER_BELONGS_TO_GROUP:
      str = "No activate for an individual peer-group member is invalid";
      break;
    case BGP_ERR_PEER_GROUP_AF_UNCONFIGURED:
      str = "Activate the peer-group for the address family first";
      break;
    case BGP_ERR_PEER_GROUP_NO_REMOTE_AS:
      str = "Specify remote-as or peer-group remote AS first";
      break;
    case BGP_ERR_PEER_GROUP_CANT_CHANGE:
      str = "Cannot change the peer-group. Deconfigure first";
      break;
    case BGP_ERR_PEER_GROUP_MISMATCH:
      str = "Cannot have different peer-group for the neighbor";
      break;
    case BGP_ERR_PEER_FILTER_CONFLICT:
      str = "Prefix/distribute list can not co-exist";
      break;
    case BGP_ERR_NOT_INTERNAL_PEER:
      str = "Invalid command. Not an internal neighbor";
      break;
    case BGP_ERR_REMOVE_PRIVATE_AS:
      str = "Private AS cannot be removed for IBGP peers";
      break;
    case BGP_ERR_LOCAL_AS_ALLOWED_ONLY_FOR_EBGP:
      str = "Local-AS allowed only for EBGP peers";
      break;
    case BGP_ERR_CANNOT_HAVE_LOCAL_AS_SAME_AS:
      str = "Cannot have local-as same as BGP AS number";
      break;
    }
  if (str)
    {
      vty_out (vty, "%% %s%s", str, VTY_NEWLINE);
      return CMD_WARNING;
    }
  return CMD_SUCCESS;
}