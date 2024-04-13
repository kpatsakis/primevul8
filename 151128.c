flatpak_context_allow_host_fs (FlatpakContext *context)
{
  flatpak_context_add_filesystem (context, "host");
}