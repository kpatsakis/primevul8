static void lo_fsync(fuse_req_t req, fuse_ino_t ino, int datasync,
                     struct fuse_file_info *fi)
{
    struct lo_inode *inode = lo_inode(req, ino);
    struct lo_data *lo = lo_data(req);
    int res;
    int fd;

    fuse_log(FUSE_LOG_DEBUG, "lo_fsync(ino=%" PRIu64 ", fi=0x%p)\n", ino,
             (void *)fi);

    if (!inode) {
        fuse_reply_err(req, EBADF);
        return;
    }

    if (!fi) {
        fd = lo_inode_open(lo, inode, O_RDWR);
        if (fd < 0) {
            res = -fd;
            goto out;
        }
    } else {
        fd = lo_fi_fd(req, fi);
    }

    if (datasync) {
        res = fdatasync(fd) == -1 ? errno : 0;
    } else {
        res = fsync(fd) == -1 ? errno : 0;
    }
    if (!fi) {
        close(fd);
    }
out:
    lo_inode_put(lo, &inode);
    fuse_reply_err(req, res);
}