add_font_path_args (GPtrArray *argv_array)
{
  g_autoptr(GFile) home = NULL;
  g_autoptr(GFile) user_font1 = NULL;
  g_autoptr(GFile) user_font2 = NULL;

  if (g_file_test (SYSTEM_FONTS_DIR, G_FILE_TEST_EXISTS))
    {
      add_args (argv_array,
                "--bind", SYSTEM_FONTS_DIR, "/run/host/fonts",
                NULL);
    }

  home = g_file_new_for_path (g_get_home_dir ());
  user_font1 = g_file_resolve_relative_path (home, ".local/share/fonts");
  user_font2 = g_file_resolve_relative_path (home, ".fonts");

  if (g_file_query_exists (user_font1, NULL))
    {
      add_args (argv_array,
                "--bind", flatpak_file_get_path_cached (user_font1), "/run/host/user-fonts",
                NULL);
    }
  else if (g_file_query_exists (user_font2, NULL))
    {
      add_args (argv_array,
                "--bind", flatpak_file_get_path_cached (user_font2), "/run/host/user-fonts",
                NULL);
    }
}