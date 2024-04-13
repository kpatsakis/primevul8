flatpak_context_remove_features (FlatpakContext       *context,
                               FlatpakContextFeatures  features)
{
  context->features_valid |= features;
  context->features &= ~features;
}