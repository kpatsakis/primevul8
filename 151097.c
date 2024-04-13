flatpak_context_complete (FlatpakContext *context, FlatpakCompletion *completion)
{
  flatpak_complete_options (completion, context_options);
}