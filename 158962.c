static int do_statx(struct lo_data *lo, int dirfd, const char *pathname,
                    struct stat *statbuf, int flags, uint64_t *mnt_id)
{
    int res;

#if defined(CONFIG_STATX) && defined(STATX_MNT_ID)
    if (lo->use_statx) {
        struct statx statxbuf;

        res = statx(dirfd, pathname, flags, STATX_BASIC_STATS | STATX_MNT_ID,
                    &statxbuf);
        if (!res) {
            memset(statbuf, 0, sizeof(*statbuf));
            statbuf->st_dev = makedev(statxbuf.stx_dev_major,
                                      statxbuf.stx_dev_minor);
            statbuf->st_ino = statxbuf.stx_ino;
            statbuf->st_mode = statxbuf.stx_mode;
            statbuf->st_nlink = statxbuf.stx_nlink;
            statbuf->st_uid = statxbuf.stx_uid;
            statbuf->st_gid = statxbuf.stx_gid;
            statbuf->st_rdev = makedev(statxbuf.stx_rdev_major,
                                       statxbuf.stx_rdev_minor);
            statbuf->st_size = statxbuf.stx_size;
            statbuf->st_blksize = statxbuf.stx_blksize;
            statbuf->st_blocks = statxbuf.stx_blocks;
            statbuf->st_atim.tv_sec = statxbuf.stx_atime.tv_sec;
            statbuf->st_atim.tv_nsec = statxbuf.stx_atime.tv_nsec;
            statbuf->st_mtim.tv_sec = statxbuf.stx_mtime.tv_sec;
            statbuf->st_mtim.tv_nsec = statxbuf.stx_mtime.tv_nsec;
            statbuf->st_ctim.tv_sec = statxbuf.stx_ctime.tv_sec;
            statbuf->st_ctim.tv_nsec = statxbuf.stx_ctime.tv_nsec;

            if (statxbuf.stx_mask & STATX_MNT_ID) {
                *mnt_id = statxbuf.stx_mnt_id;
            } else {
                *mnt_id = 0;
            }
            return 0;
        } else if (errno != ENOSYS) {
            return -1;
        }
        lo->use_statx = false;
        /* fallback */
    }
#endif
    res = fstatat(dirfd, pathname, statbuf, flags);
    if (res == -1) {
        return -1;
    }
    *mnt_id = 0;

    return 0;
}