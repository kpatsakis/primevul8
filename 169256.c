bgp_attr_flags_diagnose (struct bgp_attr_parser_args *args,
                         u_int8_t desired_flags /* how RFC says it must be */
)
{
  u_char seen = 0, i;
  u_char real_flags = args->flags;
  const u_int8_t attr_code = args->type;
  
  desired_flags &= ~BGP_ATTR_FLAG_EXTLEN;
  real_flags &= ~BGP_ATTR_FLAG_EXTLEN;
  for (i = 0; i <= 2; i++) /* O,T,P, but not E */
    if
    (
      CHECK_FLAG (desired_flags, attr_flag_str[i].key) !=
      CHECK_FLAG (real_flags,    attr_flag_str[i].key)
    )
    {
      zlog (args->peer->log, LOG_ERR, "%s attribute must%s be flagged as \"%s\"",
            LOOKUP (attr_str, attr_code),
            CHECK_FLAG (desired_flags, attr_flag_str[i].key) ? "" : " not",
            attr_flag_str[i].str);
      seen = 1;
    }
  if (!seen)
    {
      zlog (args->peer->log, LOG_DEBUG,
            "Strange, %s called for attr %s, but no problem found with flags"
            " (real flags 0x%x, desired 0x%x)",
            __func__, LOOKUP (attr_str, attr_code),
            real_flags, desired_flags);
    }
}