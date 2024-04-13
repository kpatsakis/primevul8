options_transports(void)
{
struct transport_info * ti;
uschar buf[64];

options_from_list(optionlist_transports, nelem(optionlist_transports), US"TRANSPORTS", NULL);

for (ti = transports_available; ti->driver_name[0]; ti++)
  {
  spf(buf, sizeof(buf), US"_DRIVER_TRANSPORT_%T", ti->driver_name);
  builtin_macro_create(buf);
  options_from_list(ti->options, (unsigned)*ti->options_count, US"TRANSPORT", ti->driver_name);
  }
}