flatpak_context_remove_filesystem (FlatpakContext *context,
                                   const char     *what)
{
  g_hash_table_insert (context->filesystems,
                       parse_filesystem_flags (what, NULL),
                       NULL);
}