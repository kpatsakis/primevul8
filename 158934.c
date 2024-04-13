static int lo_inode_open(struct lo_data *lo, struct lo_inode *inode,
                         int open_flags)
{
    g_autofree char *fd_str = g_strdup_printf("%d", inode->fd);
    int fd;

    if (!S_ISREG(inode->filetype) && !S_ISDIR(inode->filetype)) {
        return -EBADF;
    }

    /*
     * The file is a symlink so O_NOFOLLOW must be ignored. We checked earlier
     * that the inode is not a special file but if an external process races
     * with us then symlinks are traversed here. It is not possible to escape
     * the shared directory since it is mounted as "/" though.
     */
    fd = openat(lo->proc_self_fd, fd_str, open_flags & ~O_NOFOLLOW);
    if (fd < 0) {
        return -errno;
    }
    return fd;
}