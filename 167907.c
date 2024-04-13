vg_handle_cursor(VuDev *dev, int qidx)
{
    VuGpu *g = container_of(dev, VuGpu, dev.parent);
    VuVirtq *vq = vu_get_queue(dev, qidx);
    VuVirtqElement *elem;
    size_t len;
    struct virtio_gpu_update_cursor cursor;

    for (;;) {
        elem = vu_queue_pop(dev, vq, sizeof(VuVirtqElement));
        if (!elem) {
            break;
        }
        g_debug("cursor out:%d in:%d\n", elem->out_num, elem->in_num);

        len = iov_to_buf(elem->out_sg, elem->out_num,
                         0, &cursor, sizeof(cursor));
        if (len != sizeof(cursor)) {
            g_warning("%s: cursor size incorrect %zu vs %zu\n",
                      __func__, len, sizeof(cursor));
        } else {
            virtio_gpu_bswap_32(&cursor, sizeof(cursor));
            vg_process_cursor_cmd(g, &cursor);
        }
        vu_queue_push(dev, vq, elem, 0);
        vu_queue_notify(dev, vq);
        free(elem);
    }
}