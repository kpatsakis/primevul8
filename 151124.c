flatpak_context_add_features (FlatpakContext       *context,
                           FlatpakContextFeatures   features)
{
  context->features_valid |= features;
  context->features |= features;
}