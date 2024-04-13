static void setup_chroot(struct lo_data *lo)
{
    lo->proc_self_fd = open("/proc/self/fd", O_PATH);
    if (lo->proc_self_fd == -1) {
        fuse_log(FUSE_LOG_ERR, "open(\"/proc/self/fd\", O_PATH): %m\n");
        exit(1);
    }

    /*
     * Make the shared directory the file system root so that FUSE_OPEN
     * (lo_open()) cannot escape the shared directory by opening a symlink.
     *
     * The chroot(2) syscall is later disabled by seccomp and the
     * CAP_SYS_CHROOT capability is dropped so that tampering with the chroot
     * is not possible.
     *
     * However, it's still possible to escape the chroot via lo->proc_self_fd
     * but that requires first gaining control of the process.
     */
    if (chroot(lo->source) != 0) {
        fuse_log(FUSE_LOG_ERR, "chroot(\"%s\"): %m\n", lo->source);
        exit(1);
    }

    /* Move into the chroot */
    if (chdir("/") != 0) {
        fuse_log(FUSE_LOG_ERR, "chdir(\"/\"): %m\n");
        exit(1);
    }
}