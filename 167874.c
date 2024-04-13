vg_resource_flush(VuGpu *g,
                  struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_simple_resource *res;
    struct virtio_gpu_resource_flush rf;
    pixman_region16_t flush_region;
    int i;

    VUGPU_FILL_CMD(rf);
    virtio_gpu_bswap_32(&rf, sizeof(rf));

    res = virtio_gpu_find_resource(g, rf.resource_id);
    if (!res) {
        g_critical("%s: illegal resource specified %d\n",
                   __func__, rf.resource_id);
        cmd->error = VIRTIO_GPU_RESP_ERR_INVALID_RESOURCE_ID;
        return;
    }

    if (rf.r.x > res->width ||
        rf.r.y > res->height ||
        rf.r.width > res->width ||
        rf.r.height > res->height ||
        rf.r.x + rf.r.width > res->width ||
        rf.r.y + rf.r.height > res->height) {
        g_critical("%s: flush bounds outside resource"
                   " bounds for resource %d: %d %d %d %d vs %d %d\n",
                   __func__, rf.resource_id, rf.r.x, rf.r.y,
                   rf.r.width, rf.r.height, res->width, res->height);
        cmd->error = VIRTIO_GPU_RESP_ERR_INVALID_PARAMETER;
        return;
    }

    pixman_region_init_rect(&flush_region,
                            rf.r.x, rf.r.y, rf.r.width, rf.r.height);
    for (i = 0; i < VIRTIO_GPU_MAX_SCANOUTS; i++) {
        struct virtio_gpu_scanout *scanout;
        pixman_region16_t region, finalregion;
        pixman_box16_t *extents;

        if (!(res->scanout_bitmask & (1 << i))) {
            continue;
        }
        scanout = &g->scanout[i];

        pixman_region_init(&finalregion);
        pixman_region_init_rect(&region, scanout->x, scanout->y,
                                scanout->width, scanout->height);

        pixman_region_intersect(&finalregion, &flush_region, &region);

        extents = pixman_region_extents(&finalregion);
        size_t width = extents->x2 - extents->x1;
        size_t height = extents->y2 - extents->y1;

        if (vugbm_buffer_can_get_dmabuf_fd(&res->buffer)) {
            VhostUserGpuMsg vmsg = {
                .request = VHOST_USER_GPU_DMABUF_UPDATE,
                .size = sizeof(VhostUserGpuUpdate),
                .payload.update = (VhostUserGpuUpdate) {
                    .scanout_id = i,
                    .x = extents->x1,
                    .y = extents->y1,
                    .width = width,
                    .height = height,
                }
            };
            vg_send_msg(g, &vmsg, -1);
            vg_wait_ok(g);
        } else {
            size_t bpp =
                PIXMAN_FORMAT_BPP(pixman_image_get_format(res->image)) / 8;
            size_t size = width * height * bpp;

            void *p = g_malloc(VHOST_USER_GPU_HDR_SIZE +
                               sizeof(VhostUserGpuUpdate) + size);
            VhostUserGpuMsg *msg = p;
            msg->request = VHOST_USER_GPU_UPDATE;
            msg->size = sizeof(VhostUserGpuUpdate) + size;
            msg->payload.update = (VhostUserGpuUpdate) {
                .scanout_id = i,
                .x = extents->x1,
                .y = extents->y1,
                .width = width,
                .height = height,
            };
            pixman_image_t *i =
                pixman_image_create_bits(pixman_image_get_format(res->image),
                                         msg->payload.update.width,
                                         msg->payload.update.height,
                                         p + offsetof(VhostUserGpuMsg,
                                                      payload.update.data),
                                         width * bpp);
            pixman_image_composite(PIXMAN_OP_SRC,
                                   res->image, NULL, i,
                                   extents->x1, extents->y1,
                                   0, 0, 0, 0,
                                   width, height);
            pixman_image_unref(i);
            vg_send_msg(g, msg, -1);
            g_free(msg);
        }
        pixman_region_fini(&region);
        pixman_region_fini(&finalregion);
    }
    pixman_region_fini(&flush_region);
}