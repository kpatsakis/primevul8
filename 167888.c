virtio_gpu_find_resource(VuGpu *g, uint32_t resource_id)
{
    struct virtio_gpu_simple_resource *res;

    QTAILQ_FOREACH(res, &g->reslist, next) {
        if (res->resource_id == resource_id) {
            return res;
        }
    }
    return NULL;
}