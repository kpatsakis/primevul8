static void lo_create(fuse_req_t req, fuse_ino_t parent, const char *name,
                      mode_t mode, struct fuse_file_info *fi)
{
    int fd = -1;
    struct lo_data *lo = lo_data(req);
    struct lo_inode *parent_inode;
    struct lo_inode *inode = NULL;
    struct fuse_entry_param e;
    int err;
    struct lo_cred old = {};

    fuse_log(FUSE_LOG_DEBUG, "lo_create(parent=%" PRIu64 ", name=%s)"
             " kill_priv=%d\n", parent, name, fi->kill_priv);

    if (!is_safe_path_component(name)) {
        fuse_reply_err(req, EINVAL);
        return;
    }

    parent_inode = lo_inode(req, parent);
    if (!parent_inode) {
        fuse_reply_err(req, EBADF);
        return;
    }

    err = lo_change_cred(req, &old, lo->change_umask);
    if (err) {
        goto out;
    }

    update_open_flags(lo->writeback, lo->allow_direct_io, fi);

    /* Try to create a new file but don't open existing files */
    fd = openat(parent_inode->fd, name, fi->flags | O_CREAT | O_EXCL, mode);
    err = fd == -1 ? errno : 0;

    lo_restore_cred(&old, lo->change_umask);

    /* Ignore the error if file exists and O_EXCL was not given */
    if (err && (err != EEXIST || (fi->flags & O_EXCL))) {
        goto out;
    }

    err = lo_do_lookup(req, parent, name, &e, &inode);
    if (err) {
        goto out;
    }

    err = lo_do_open(lo, inode, fd, fi);
    fd = -1; /* lo_do_open() takes ownership of fd */
    if (err) {
        /* Undo lo_do_lookup() nlookup ref */
        unref_inode_lolocked(lo, inode, 1);
    }

out:
    lo_inode_put(lo, &inode);
    lo_inode_put(lo, &parent_inode);

    if (err) {
        if (fd >= 0) {
            close(fd);
        }

        fuse_reply_err(req, err);
    } else {
        fuse_reply_create(req, &e, fi);
    }
}