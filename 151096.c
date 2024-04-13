add_default_permissions (FlatpakContext *app_context)
{
  flatpak_context_set_session_bus_policy (app_context,
                                          "org.freedesktop.portal.*",
                                          FLATPAK_POLICY_TALK);
}