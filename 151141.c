flatpak_context_shared_to_args (FlatpakContextShares shares,
                                FlatpakContextShares valid,
                                GPtrArray *args)
{
  return flatpak_context_bitmask_to_args (shares, valid, flatpak_context_shares, "--share", "--unshare", args);
}