log_config_info(gstring * g, int flags)
{
g = string_cat(g, US"Exim configuration error");

if (flags & (LOG_CONFIG_FOR & ~LOG_CONFIG))
  return string_cat(g, US" for ");

if (flags & (LOG_CONFIG_IN & ~LOG_CONFIG))
  g = string_fmt_append(g, " in line %d of %s", config_lineno, config_filename);

return string_catn(g, US":\n  ", 4);
}