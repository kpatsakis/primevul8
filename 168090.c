nm_utils_setpgid(gpointer unused G_GNUC_UNUSED)
{
    pid_t pid;

    pid = getpid();
    setpgid(pid, pid);
}