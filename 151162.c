clear_fd (gpointer data)
{
  int *fd_p = data;
  if (fd_p != NULL && *fd_p != -1)
    close (*fd_p);
}