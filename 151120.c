dbus_spawn_child_setup (gpointer user_data)
{
  DbusProxySpawnData *data = user_data;

  /* Unset CLOEXEC */
  fcntl (data->sync_fd, F_SETFD, 0);
  fcntl (data->app_info_fd, F_SETFD, 0);
  fcntl (data->bwrap_args_fd, F_SETFD, 0);
}