static void setup_root(struct lo_data *lo, struct lo_inode *root)
{
    int fd, res;
    struct stat stat;
    uint64_t mnt_id;

    fd = open("/", O_PATH);
    if (fd == -1) {
        fuse_log(FUSE_LOG_ERR, "open(%s, O_PATH): %m\n", lo->source);
        exit(1);
    }

    res = do_statx(lo, fd, "", &stat, AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW,
                   &mnt_id);
    if (res == -1) {
        fuse_log(FUSE_LOG_ERR, "fstatat(%s): %m\n", lo->source);
        exit(1);
    }

    root->filetype = S_IFDIR;
    root->fd = fd;
    root->key.ino = stat.st_ino;
    root->key.dev = stat.st_dev;
    root->key.mnt_id = mnt_id;
    root->nlookup = 2;
    g_atomic_int_set(&root->refcount, 2);
    if (lo->posix_lock) {
        pthread_mutex_init(&root->plock_mutex, NULL);
        root->posix_locks = g_hash_table_new_full(
            g_direct_hash, g_direct_equal, NULL, posix_locks_value_destroy);
    }
}