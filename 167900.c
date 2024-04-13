vg_ctrl_response(VuGpu *g,
                 struct virtio_gpu_ctrl_command *cmd,
                 struct virtio_gpu_ctrl_hdr *resp,
                 size_t resp_len)
{
    size_t s;

    if (cmd->cmd_hdr.flags & VIRTIO_GPU_FLAG_FENCE) {
        resp->flags |= VIRTIO_GPU_FLAG_FENCE;
        resp->fence_id = cmd->cmd_hdr.fence_id;
        resp->ctx_id = cmd->cmd_hdr.ctx_id;
    }
    virtio_gpu_ctrl_hdr_bswap(resp);
    s = iov_from_buf(cmd->elem.in_sg, cmd->elem.in_num, 0, resp, resp_len);
    if (s != resp_len) {
        g_critical("%s: response size incorrect %zu vs %zu",
                   __func__, s, resp_len);
    }
    vu_queue_push(&g->dev.parent, cmd->vq, &cmd->elem, s);
    vu_queue_notify(&g->dev.parent, cmd->vq);
    cmd->state = VG_CMD_STATE_FINISHED;
}