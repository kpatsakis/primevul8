vg_create_mapping_iov(VuGpu *g,
                      struct virtio_gpu_resource_attach_backing *ab,
                      struct virtio_gpu_ctrl_command *cmd,
                      struct iovec **iov)
{
    struct virtio_gpu_mem_entry *ents;
    size_t esize, s;
    int i;

    if (ab->nr_entries > 16384) {
        g_critical("%s: nr_entries is too big (%d > 16384)",
                   __func__, ab->nr_entries);
        return -1;
    }

    esize = sizeof(*ents) * ab->nr_entries;
    ents = g_malloc(esize);
    s = iov_to_buf(cmd->elem.out_sg, cmd->elem.out_num,
                   sizeof(*ab), ents, esize);
    if (s != esize) {
        g_critical("%s: command data size incorrect %zu vs %zu",
                   __func__, s, esize);
        g_free(ents);
        return -1;
    }

    *iov = g_malloc0(sizeof(struct iovec) * ab->nr_entries);
    for (i = 0; i < ab->nr_entries; i++) {
        uint64_t len = ents[i].length;
        (*iov)[i].iov_len = ents[i].length;
        (*iov)[i].iov_base = vu_gpa_to_va(&g->dev.parent, &len, ents[i].addr);
        if (!(*iov)[i].iov_base || len != ents[i].length) {
            g_critical("%s: resource %d element %d",
                       __func__, ab->resource_id, i);
            g_free(*iov);
            g_free(ents);
            *iov = NULL;
            return -1;
        }
    }
    g_free(ents);
    return 0;
}