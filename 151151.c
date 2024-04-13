flatpak_run_apply_env_appid (char **envp,
                             GFile *app_dir)
{
  g_autoptr(GFile) app_dir_data = NULL;
  g_autoptr(GFile) app_dir_config = NULL;
  g_autoptr(GFile) app_dir_cache = NULL;

  app_dir_data = g_file_get_child (app_dir, "data");
  app_dir_config = g_file_get_child (app_dir, "config");
  app_dir_cache = g_file_get_child (app_dir, "cache");
  envp = g_environ_setenv (envp, "XDG_DATA_HOME", flatpak_file_get_path_cached (app_dir_data), TRUE);
  envp = g_environ_setenv (envp, "XDG_CONFIG_HOME", flatpak_file_get_path_cached (app_dir_config), TRUE);
  envp = g_environ_setenv (envp, "XDG_CACHE_HOME", flatpak_file_get_path_cached (app_dir_cache), TRUE);

  return envp;
}