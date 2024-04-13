flatpak_run_get_minimal_env (gboolean devel)
{
  GPtrArray *env_array;
  static const char * const copy[] = {
    "PWD",
    "GDMSESSION",
    "XDG_CURRENT_DESKTOP",
    "XDG_SESSION_DESKTOP",
    "DESKTOP_SESSION",
    "EMAIL_ADDRESS",
    "HOME",
    "HOSTNAME",
    "LOGNAME",
    "REAL_NAME",
    "TERM",
    "USER",
    "USERNAME",
  };
  static const char * const copy_nodevel[] = {
    "LANG",
    "LANGUAGE",
    "LC_ALL",
    "LC_ADDRESS",
    "LC_COLLATE",
    "LC_CTYPE",
    "LC_IDENTIFICATION",
    "LC_MEASUREMENT",
    "LC_MESSAGES",
    "LC_MONETARY",
    "LC_NAME",
    "LC_NUMERIC",
    "LC_PAPER",
    "LC_TELEPHONE",
    "LC_TIME",
  };
  int i;

  env_array = g_ptr_array_new_with_free_func (g_free);

  for (i = 0; i < G_N_ELEMENTS (default_exports); i++)
    g_ptr_array_add (env_array, g_strdup_printf ("%s=%s", default_exports[i].env, default_exports[i].val));

  if (devel)
    {
      for (i = 0; i < G_N_ELEMENTS(devel_exports); i++)
        g_ptr_array_add (env_array, g_strdup_printf ("%s=%s", devel_exports[i].env, devel_exports[i].val));
    }

  for (i = 0; i < G_N_ELEMENTS (copy); i++)
    {
      const char *current = g_getenv (copy[i]);
      if (current)
        g_ptr_array_add (env_array, g_strdup_printf ("%s=%s", copy[i], current));
    }

  if (!devel)
    {
      for (i = 0; i < G_N_ELEMENTS (copy_nodevel); i++)
        {
          const char *current = g_getenv (copy_nodevel[i]);
          if (current)
            g_ptr_array_add (env_array, g_strdup_printf ("%s=%s", copy_nodevel[i], current));
        }
    }

  g_ptr_array_add (env_array, NULL);
  return (char **) g_ptr_array_free (env_array, FALSE);
}