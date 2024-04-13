flatpak_context_features_to_args (FlatpakContextFeatures features,
                                FlatpakContextFeatures valid,
                                GPtrArray *args)
{
  return flatpak_context_bitmask_to_args (features, valid, flatpak_context_features, "--allow", "--disallow", args);
}