child_setup (gpointer user_data)
{
  GArray *fd_array = user_data;
  int i;

  /* If no fd_array was specified, don't care. */
  if (fd_array == NULL)
    return;

  /* Otherwise, mark not - close-on-exec all the fds in the array */
  for (i = 0; i < fd_array->len; i++)
    fcntl (g_array_index (fd_array, int, i), F_SETFD, 0);
}