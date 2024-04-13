zroute_lookup(u_int zroute)
{
  u_int i;

  if (zroute >= sizeof(route_types)/sizeof(route_types[0]))
    {
      zlog_err("unknown zebra route type: %u", zroute);
      return &unknown;
    }
  if (zroute == route_types[zroute].type)
    return &route_types[zroute];
  for (i = 0; i < sizeof(route_types)/sizeof(route_types[0]); i++)
    {
      if (zroute == route_types[i].type)
        {
	  zlog_warn("internal error: route type table out of order "
		    "while searching for %u, please notify developers", zroute);
	  return &route_types[i];
        }
    }
  zlog_err("internal error: cannot find route type %u in table!", zroute);
  return &unknown;
}