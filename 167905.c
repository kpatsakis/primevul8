vg_queue_set_started(VuDev *dev, int qidx, bool started)
{
    VuVirtq *vq = vu_get_queue(dev, qidx);

    g_debug("queue started %d:%d\n", qidx, started);

    switch (qidx) {
    case 0:
        vu_set_queue_handler(dev, vq, started ? vg_handle_ctrl : NULL);
        break;
    case 1:
        vu_set_queue_handler(dev, vq, started ? vg_handle_cursor : NULL);
        break;
    default:
        break;
    }
}