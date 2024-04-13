bgp_str2route_type (int afi, const char *str)
{
  if (! str)
    return 0;

  if (afi == AFI_IP)
    {
      if (strncmp (str, "k", 1) == 0)
	return ZEBRA_ROUTE_KERNEL;
      else if (strncmp (str, "c", 1) == 0)
	return ZEBRA_ROUTE_CONNECT;
      else if (strncmp (str, "s", 1) == 0)
	return ZEBRA_ROUTE_STATIC;
      else if (strncmp (str, "r", 1) == 0)
	return ZEBRA_ROUTE_RIP;
      else if (strncmp (str, "o", 1) == 0)
	return ZEBRA_ROUTE_OSPF;
    }
  if (afi == AFI_IP6)
    {
      if (strncmp (str, "k", 1) == 0)
	return ZEBRA_ROUTE_KERNEL;
      else if (strncmp (str, "c", 1) == 0)
	return ZEBRA_ROUTE_CONNECT;
      else if (strncmp (str, "s", 1) == 0)
	return ZEBRA_ROUTE_STATIC;
      else if (strncmp (str, "r", 1) == 0)
	return ZEBRA_ROUTE_RIPNG;
      else if (strncmp (str, "o", 1) == 0)
	return ZEBRA_ROUTE_OSPF6;
    }
  return 0;
}