ieee802154_match_sock(__le64 hw_addr, __le16 pan_id, __le16 short_addr,
		      struct dgram_sock *ro)
{
	if (!ro->bound)
		return true;

	if (ro->src_addr.mode == IEEE802154_ADDR_LONG &&
	    hw_addr == ro->src_addr.extended_addr)
		return true;

	if (ro->src_addr.mode == IEEE802154_ADDR_SHORT &&
	    pan_id == ro->src_addr.pan_id &&
	    short_addr == ro->src_addr.short_addr)
		return true;

	return false;
}