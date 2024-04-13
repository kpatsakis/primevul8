static struct lo_inode *lo_find(struct lo_data *lo, struct stat *st,
                                uint64_t mnt_id)
{
    struct lo_inode *p;
    struct lo_key key = {
        .ino = st->st_ino,
        .dev = st->st_dev,
        .mnt_id = mnt_id,
    };

    pthread_mutex_lock(&lo->mutex);
    p = g_hash_table_lookup(lo->inodes, &key);
    if (p) {
        assert(p->nlookup > 0);
        p->nlookup++;
        g_atomic_int_inc(&p->refcount);
    }
    pthread_mutex_unlock(&lo->mutex);

    return p;
}