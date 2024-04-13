static inline item* limited_get(char *key, size_t nkey, conn *c, uint32_t exptime, bool should_touch, bool do_update, bool *overflow) {
    item *it;
    if (should_touch) {
        it = item_touch(key, nkey, exptime, c);
    } else {
        it = item_get(key, nkey, c, do_update);
    }
    if (it && it->refcount > IT_REFCOUNT_LIMIT) {
        item_remove(it);
        it = NULL;
        *overflow = true;
    } else {
        *overflow = false;
    }
    return it;
}