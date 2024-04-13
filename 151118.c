option_add_generic_policy_cb (const gchar *option_name,
                              const gchar *value,
                              gpointer     data,
                              GError     **error)
{
  FlatpakContext *context = data;
  char *t;
  g_autofree char *key = NULL;
  const char *policy_value;

  t = strchr (value, '=');
  if (t == NULL)
    return flatpak_fail (error, "--policy arguments must be in the form SUBSYSTEM.KEY=[!]VALUE");
  policy_value = t + 1;
  key = g_strndup (value, t - value);
  if (strchr (key, '.') == NULL)
    return flatpak_fail (error, "--policy arguments must be in the form SUBSYSTEM.KEY=[!]VALUE");

  if (policy_value[0] == '!')
    return flatpak_fail (error, "--policy values can't start with \"!\"");

  flatpak_context_apply_generic_policy (context, key, policy_value);

  return TRUE;
}