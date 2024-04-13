prepend_bwrap_argv_wrapper (GPtrArray *argv,
                            int app_info_fd,
                            int *bwrap_fd_out,
                            GError **error)
{
  int i = 0;
  g_auto(GLnxDirFdIterator) dir_iter = { 0 };
  struct dirent *dent;
  g_autoptr(GPtrArray) bwrap_args = g_ptr_array_new_with_free_func (g_free);
  gsize bwrap_args_len;
  glnx_fd_close int bwrap_args_fd = -1;
  g_autofree char *bwrap_args_data = NULL;

  if (!glnx_dirfd_iterator_init_at (AT_FDCWD, "/", FALSE, &dir_iter, error))
    return FALSE;

  while (TRUE)
    {
      if (!glnx_dirfd_iterator_next_dent_ensure_dtype (&dir_iter, &dent, NULL, error))
        return FALSE;

      if (dent == NULL)
        break;

      if (strcmp (dent->d_name, ".flatpak-info") == 0)
        continue;

      if (dent->d_type == DT_DIR)
        {
          if (strcmp (dent->d_name, "tmp") == 0 ||
              strcmp (dent->d_name, "var") == 0 ||
              strcmp (dent->d_name, "run") == 0)
            g_ptr_array_add (bwrap_args, g_strdup ("--bind"));
          else
            g_ptr_array_add (bwrap_args, g_strdup ("--ro-bind"));
          g_ptr_array_add (bwrap_args, g_strconcat ("/", dent->d_name, NULL));
          g_ptr_array_add (bwrap_args, g_strconcat ("/", dent->d_name, NULL));
        }
      else if (dent->d_type == DT_LNK)
        {
          ssize_t symlink_size;
          char path_buffer[PATH_MAX + 1];

          symlink_size = readlinkat (dir_iter.fd, dent->d_name, path_buffer, sizeof (path_buffer) - 1);
          if (symlink_size < 0)
            {
              glnx_set_error_from_errno (error);
              return FALSE;
            }
          path_buffer[symlink_size] = 0;

          g_ptr_array_add (bwrap_args, g_strdup ("--symlink"));
          g_ptr_array_add (bwrap_args, g_strdup (path_buffer));
          g_ptr_array_add (bwrap_args, g_strconcat ("/", dent->d_name, NULL));
        }
    }

  g_ptr_array_add (bwrap_args, g_strdup ("--ro-bind-data"));
  g_ptr_array_add (bwrap_args, g_strdup_printf ("%d", app_info_fd));
  g_ptr_array_add (bwrap_args, g_strdup ("/.flatpak-info"));

  bwrap_args_data = join_args (bwrap_args, &bwrap_args_len);
  bwrap_args_fd = create_tmp_fd (bwrap_args_data, bwrap_args_len, error);
  if (bwrap_args_fd < 0)
    return FALSE;

  g_ptr_array_insert (argv, i++, g_strdup (flatpak_get_bwrap ()));
  g_ptr_array_insert (argv, i++, g_strdup ("--args"));
  g_ptr_array_insert (argv, i++, g_strdup_printf ("%d", bwrap_args_fd));

  *bwrap_fd_out = bwrap_args_fd;
  bwrap_args_fd = -1; /* Steal it */

  return TRUE;
}