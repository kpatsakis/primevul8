flatpak_run_apply_env_default (char **envp)
{
  int i;

  for (i = 0; i < G_N_ELEMENTS (default_exports); i++)
    envp = g_environ_setenv (envp, default_exports[i].env, default_exports[i].val, TRUE);

  return envp;
}