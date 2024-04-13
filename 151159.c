flatpak_context_add_filesystem (FlatpakContext *context,
                                const char     *what)
{
  FlatpakFilesystemMode mode;
  char *fs = parse_filesystem_flags (what, &mode);

  g_hash_table_insert (context->filesystems, fs, GINT_TO_POINTER (mode));
}