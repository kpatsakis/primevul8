gx_device_finalize(const gs_memory_t *cmem, void *vptr)
{
    gx_device * const dev = (gx_device *)vptr;
    (void)cmem; /* unused */

    if (dev->icc_struct != NULL) {
        rc_decrement(dev->icc_struct, "gx_device_finalize(icc_profile)");
    }
    if (dev->finalize)
        dev->finalize(dev);

    /* Deal with subclassed devices. Ordinarily these should not be a problem, we
     * will never see them, but if ths is a end of job restore we can end up
     * with the 'child' device(s) being freed before their parents. We need to make
     * sure we don't leave any dangling pointers in that case.
     */
    if (dev->child)
        dev->child->parent = dev->parent;
    if (dev->parent)
        dev->parent->child = dev->child;
    if (dev->PageList) {
        rc_decrement(dev->PageList, "gx_device_finalize(PageList)");
        dev->PageList = 0;
    }

    discard(gs_closedevice(dev));
    if (dev->stype_is_dynamic)
        gs_free_const_object(dev->memory->non_gc_memory, dev->stype,
                             "gx_device_finalize");

#ifdef DEBUG
    /* Slightly ugly hack: because the garbage collector makes no promises
     * about the order objects can be garbage collected, it is possible for
     * a forwarding device to remain in existence (awaiting garbage collection
     * itself) after it's target marked as free memory by the garbage collector.
     * In such a case, the normal reference counting is fine (since the garbage
     * collector leaves the object contents alone until is has completed its
     * sweep), but the reference counting debugging attempts to access the
     * memory header to output type information - and the header has been
     * overwritten by the garbage collector, causing a crash.
     * Setting the rc memory to NULL here should be safe, since the memory
     * is now in the hands of the garbage collector, and means we can check in
     * debugging code to ensure we don't try to use values that not longer exist
     * in the memmory header.
     * In the non-gc case, finalize is the very last thing to happen before the
     * memory is actually freed, so the rc.memory pointer is moot.
     * See rc_object_type_name()
     */
    if (gs_debug_c('^'))
        dev->rc.memory = NULL;
#endif
}