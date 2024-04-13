vg_set_scanout(VuGpu *g,
               struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_simple_resource *res, *ores;
    struct virtio_gpu_scanout *scanout;
    struct virtio_gpu_set_scanout ss;
    int fd;

    VUGPU_FILL_CMD(ss);
    virtio_gpu_bswap_32(&ss, sizeof(ss));

    if (ss.scanout_id >= VIRTIO_GPU_MAX_SCANOUTS) {
        g_critical("%s: illegal scanout id specified %d",
                   __func__, ss.scanout_id);
        cmd->error = VIRTIO_GPU_RESP_ERR_INVALID_SCANOUT_ID;
        return;
    }

    if (ss.resource_id == 0) {
        vg_disable_scanout(g, ss.scanout_id);
        return;
    }

    /* create a surface for this scanout */
    res = virtio_gpu_find_resource(g, ss.resource_id);
    if (!res) {
        g_critical("%s: illegal resource specified %d",
                      __func__, ss.resource_id);
        cmd->error = VIRTIO_GPU_RESP_ERR_INVALID_RESOURCE_ID;
        return;
    }

    if (ss.r.x > res->width ||
        ss.r.y > res->height ||
        ss.r.width > res->width ||
        ss.r.height > res->height ||
        ss.r.x + ss.r.width > res->width ||
        ss.r.y + ss.r.height > res->height) {
        g_critical("%s: illegal scanout %d bounds for"
                   " resource %d, (%d,%d)+%d,%d vs %d %d",
                   __func__, ss.scanout_id, ss.resource_id, ss.r.x, ss.r.y,
                   ss.r.width, ss.r.height, res->width, res->height);
        cmd->error = VIRTIO_GPU_RESP_ERR_INVALID_PARAMETER;
        return;
    }

    scanout = &g->scanout[ss.scanout_id];

    ores = virtio_gpu_find_resource(g, scanout->resource_id);
    if (ores) {
        ores->scanout_bitmask &= ~(1 << ss.scanout_id);
    }

    res->scanout_bitmask |= (1 << ss.scanout_id);
    scanout->resource_id = ss.resource_id;
    scanout->x = ss.r.x;
    scanout->y = ss.r.y;
    scanout->width = ss.r.width;
    scanout->height = ss.r.height;

    struct vugbm_buffer *buffer = &res->buffer;

    if (vugbm_buffer_can_get_dmabuf_fd(buffer)) {
        VhostUserGpuMsg msg = {
            .request = VHOST_USER_GPU_DMABUF_SCANOUT,
            .size = sizeof(VhostUserGpuDMABUFScanout),
            .payload.dmabuf_scanout = (VhostUserGpuDMABUFScanout) {
                .scanout_id = ss.scanout_id,
                .x = ss.r.x,
                .y = ss.r.y,
                .width = ss.r.width,
                .height = ss.r.height,
                .fd_width = buffer->width,
                .fd_height = buffer->height,
                .fd_stride = buffer->stride,
                .fd_drm_fourcc = buffer->format
            }
        };

        if (vugbm_buffer_get_dmabuf_fd(buffer, &fd)) {
            vg_send_msg(g, &msg, fd);
            close(fd);
        }
    } else {
        VhostUserGpuMsg msg = {
            .request = VHOST_USER_GPU_SCANOUT,
            .size = sizeof(VhostUserGpuScanout),
            .payload.scanout = (VhostUserGpuScanout) {
                .scanout_id = ss.scanout_id,
                .width = scanout->width,
                .height = scanout->height
            }
        };
        vg_send_msg(g, &msg, -1);
    }
}