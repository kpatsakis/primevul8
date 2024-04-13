compute_permissions (GKeyFile *app_metadata,
                     GKeyFile *runtime_metadata,
                     GError  **error)
{
  g_autoptr(FlatpakContext) app_context = NULL;

  app_context = flatpak_context_new ();

  add_default_permissions (app_context);

  if (!flatpak_context_load_metadata (app_context, runtime_metadata, error))
    return NULL;

  if (app_metadata != NULL &&
      !flatpak_context_load_metadata (app_context, app_metadata, error))
    return NULL;

  return g_steal_pointer (&app_context);
}