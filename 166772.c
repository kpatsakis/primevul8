display_to_port_type(field_display_e e)
{
	switch (e) {
	case BASE_PT_UDP:
		return PT_UDP;
	case BASE_PT_TCP:
		return PT_TCP;
	case BASE_PT_DCCP:
		return PT_DCCP;
	case BASE_PT_SCTP:
		return PT_SCTP;
	default:
		break;
	}
	return PT_NONE;
}