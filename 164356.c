static inline item* limited_get_locked(char *key, size_t nkey, conn *c, bool do_update, uint32_t *hv, bool *overflow) {
    item *it;
    it = item_get_locked(key, nkey, c, do_update, hv);
    if (it && it->refcount > IT_REFCOUNT_LIMIT) {
        do_item_remove(it);
        it = NULL;
        item_unlock(*hv);
        *overflow = true;
    } else {
        *overflow = false;
    }
    return it;
}