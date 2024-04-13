static void unref_inode(struct lo_data *lo, struct lo_inode *inode, uint64_t n)
{
    if (!inode) {
        return;
    }

    assert(inode->nlookup >= n);
    inode->nlookup -= n;
    if (!inode->nlookup) {
        lo_map_remove(&lo->ino_map, inode->fuse_ino);
        g_hash_table_remove(lo->inodes, &inode->key);
        if (lo->posix_lock) {
            if (g_hash_table_size(inode->posix_locks)) {
                fuse_log(FUSE_LOG_WARNING, "Hash table is not empty\n");
            }
            g_hash_table_destroy(inode->posix_locks);
            pthread_mutex_destroy(&inode->plock_mutex);
        }
        /* Drop our refcount from lo_do_lookup() */
        lo_inode_put(lo, &inode);
    }
}