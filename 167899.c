vg_transfer_to_host_2d(VuGpu *g,
                       struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_simple_resource *res;
    int h;
    uint32_t src_offset, dst_offset, stride;
    int bpp;
    pixman_format_code_t format;
    struct virtio_gpu_transfer_to_host_2d t2d;

    VUGPU_FILL_CMD(t2d);
    virtio_gpu_t2d_bswap(&t2d);

    res = virtio_gpu_find_resource(g, t2d.resource_id);
    if (!res || !res->iov) {
        g_critical("%s: illegal resource specified %d",
                   __func__, t2d.resource_id);
        cmd->error = VIRTIO_GPU_RESP_ERR_INVALID_RESOURCE_ID;
        return;
    }

    if (t2d.r.x > res->width ||
        t2d.r.y > res->height ||
        t2d.r.width > res->width ||
        t2d.r.height > res->height ||
        t2d.r.x + t2d.r.width > res->width ||
        t2d.r.y + t2d.r.height > res->height) {
        g_critical("%s: transfer bounds outside resource"
                   " bounds for resource %d: %d %d %d %d vs %d %d",
                   __func__, t2d.resource_id, t2d.r.x, t2d.r.y,
                   t2d.r.width, t2d.r.height, res->width, res->height);
        cmd->error = VIRTIO_GPU_RESP_ERR_INVALID_PARAMETER;
        return;
    }

    format = pixman_image_get_format(res->image);
    bpp = (PIXMAN_FORMAT_BPP(format) + 7) / 8;
    stride = pixman_image_get_stride(res->image);

    if (t2d.offset || t2d.r.x || t2d.r.y ||
        t2d.r.width != pixman_image_get_width(res->image)) {
        void *img_data = pixman_image_get_data(res->image);
        for (h = 0; h < t2d.r.height; h++) {
            src_offset = t2d.offset + stride * h;
            dst_offset = (t2d.r.y + h) * stride + (t2d.r.x * bpp);

            iov_to_buf(res->iov, res->iov_cnt, src_offset,
                       img_data
                       + dst_offset, t2d.r.width * bpp);
        }
    } else {
        iov_to_buf(res->iov, res->iov_cnt, 0,
                   pixman_image_get_data(res->image),
                   pixman_image_get_stride(res->image)
                   * pixman_image_get_height(res->image));
    }
}