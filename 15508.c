bgp_capability_vty_out (struct vty *vty, struct peer *peer)
{
  char *pnt;
  char *end;
  struct capability_mp_data mpc;
  struct capability_header *hdr;

  pnt = peer->notify.data;
  end = pnt + peer->notify.length;
  
  while (pnt < end)
    {
      if (pnt + sizeof (struct capability_mp_data) + 2 > end)
	return;
      
      hdr = (struct capability_header *)pnt;
      if (pnt + hdr->length + 2 > end)
	return;

      memcpy (&mpc, pnt + 2, sizeof(struct capability_mp_data));

      if (hdr->code == CAPABILITY_CODE_MP)
	{
	  vty_out (vty, "  Capability error for: Multi protocol ");

	  switch (ntohs (mpc.afi))
	    {
	    case AFI_IP:
	      vty_out (vty, "AFI IPv4, ");
	      break;
	    case AFI_IP6:
	      vty_out (vty, "AFI IPv6, ");
	      break;
	    default:
	      vty_out (vty, "AFI Unknown %d, ", ntohs (mpc.afi));
	      break;
	    }
	  switch (mpc.safi)
	    {
	    case SAFI_UNICAST:
	      vty_out (vty, "SAFI Unicast");
	      break;
	    case SAFI_MULTICAST:
	      vty_out (vty, "SAFI Multicast");
	      break;
	    case SAFI_UNICAST_MULTICAST:
	      vty_out (vty, "SAFI Unicast Multicast");
	      break;
	    case BGP_SAFI_VPNV4:
	      vty_out (vty, "SAFI MPLS-VPN");
	      break;
	    default:
	      vty_out (vty, "SAFI Unknown %d ", mpc.safi);
	      break;
	    }
	  vty_out (vty, "%s", VTY_NEWLINE);
	}
      else if (hdr->code >= 128)
	vty_out (vty, "  Capability error: vendor specific capability code %d",
		 hdr->code);
      else
	vty_out (vty, "  Capability error: unknown capability code %d", 
		 hdr->code);

      pnt += hdr->length + 2;
    }
}