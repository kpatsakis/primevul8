proto_name2num(const char *s)
{
   unsigned i;

   for (i=0; i<RTSIZE; ++i)
     if (strcasecmp(s, route_types[i].string) == 0)
       return route_types[i].type;
   return -1;
}