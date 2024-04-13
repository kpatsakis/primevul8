bgp_show_peer_afi_orf_cap (struct vty *vty, struct peer *p,
			   afi_t afi, safi_t safi,
			   u_int16_t adv_smcap, u_int16_t adv_rmcap,
			   u_int16_t rcv_smcap, u_int16_t rcv_rmcap)
{
  /* Send-Mode */
  if (CHECK_FLAG (p->af_cap[afi][safi], adv_smcap)
      || CHECK_FLAG (p->af_cap[afi][safi], rcv_smcap))
    {
      vty_out (vty, "      Send-mode: ");
      if (CHECK_FLAG (p->af_cap[afi][safi], adv_smcap))
	vty_out (vty, "advertised");
      if (CHECK_FLAG (p->af_cap[afi][safi], rcv_smcap))
	vty_out (vty, "%sreceived",
		 CHECK_FLAG (p->af_cap[afi][safi], adv_smcap) ?
		 ", " : "");
      vty_out (vty, "%s", VTY_NEWLINE);
    }

  /* Receive-Mode */
  if (CHECK_FLAG (p->af_cap[afi][safi], adv_rmcap)
      || CHECK_FLAG (p->af_cap[afi][safi], rcv_rmcap))
    {
      vty_out (vty, "      Receive-mode: ");
      if (CHECK_FLAG (p->af_cap[afi][safi], adv_rmcap))
	vty_out (vty, "advertised");
      if (CHECK_FLAG (p->af_cap[afi][safi], rcv_rmcap))
	vty_out (vty, "%sreceived",
		 CHECK_FLAG (p->af_cap[afi][safi], adv_rmcap) ?
		 ", " : "");
      vty_out (vty, "%s", VTY_NEWLINE);
    }
}