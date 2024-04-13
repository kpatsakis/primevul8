flatpak_run_apply_env_vars (char **envp, FlatpakContext *context)
{
  GHashTableIter iter;
  gpointer key, value;

  g_hash_table_iter_init (&iter, context->env_vars);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      const char *var = key;
      const char *val = value;

      if (val && val[0] != 0)
        envp = g_environ_setenv (envp, var, val, TRUE);
      else
        envp = g_environ_unsetenv (envp, var);
    }

  return envp;
}