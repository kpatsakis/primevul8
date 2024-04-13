zlog_set_level (struct zlog *zl, zlog_dest_t dest, int log_level)
{
  if (zl == NULL)
    zl = zlog_default;

  zl->maxlvl[dest] = log_level;
}