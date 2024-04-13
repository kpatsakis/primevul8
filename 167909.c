update_cursor_data_simple(VuGpu *g, uint32_t resource_id, gpointer data)
{
    struct virtio_gpu_simple_resource *res;

    res = virtio_gpu_find_resource(g, resource_id);
    g_return_if_fail(res != NULL);
    g_return_if_fail(pixman_image_get_width(res->image) == 64);
    g_return_if_fail(pixman_image_get_height(res->image) == 64);
    g_return_if_fail(
        PIXMAN_FORMAT_BPP(pixman_image_get_format(res->image)) == 32);

    memcpy(data, pixman_image_get_data(res->image), 64 * 64 * sizeof(uint32_t));
}