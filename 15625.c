afi2str (afi_t afi)
{
  if (afi == AFI_IP)
    return "AFI_IP";
  else if (afi == AFI_IP6)
    return "AFI_IP6";
  else
    return "Unknown AFI";
}