flatpak_context_sockets_to_args (FlatpakContextSockets sockets,
                                 FlatpakContextSockets valid,
                                 GPtrArray *args)
{
  return flatpak_context_bitmask_to_args (sockets, valid, flatpak_context_sockets, "--socket", "--nosocket", args);
}