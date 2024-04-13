vg_handle_ctrl(VuDev *dev, int qidx)
{
    VuGpu *vg = container_of(dev, VuGpu, dev.parent);
    VuVirtq *vq = vu_get_queue(dev, qidx);
    struct virtio_gpu_ctrl_command *cmd = NULL;
    size_t len;

    for (;;) {
        if (vg->wait_in != 0) {
            return;
        }

        cmd = vu_queue_pop(dev, vq, sizeof(struct virtio_gpu_ctrl_command));
        if (!cmd) {
            break;
        }
        cmd->vq = vq;
        cmd->error = 0;
        cmd->state = VG_CMD_STATE_NEW;

        len = iov_to_buf(cmd->elem.out_sg, cmd->elem.out_num,
                         0, &cmd->cmd_hdr, sizeof(cmd->cmd_hdr));
        if (len != sizeof(cmd->cmd_hdr)) {
            g_warning("%s: command size incorrect %zu vs %zu\n",
                      __func__, len, sizeof(cmd->cmd_hdr));
        }

        virtio_gpu_ctrl_hdr_bswap(&cmd->cmd_hdr);
        g_debug("%d %s\n", cmd->cmd_hdr.type,
                vg_cmd_to_string(cmd->cmd_hdr.type));

        if (vg->virgl) {
            vg_virgl_process_cmd(vg, cmd);
        } else {
            vg_process_cmd(vg, cmd);
        }

        if (cmd->state != VG_CMD_STATE_FINISHED) {
            QTAILQ_INSERT_TAIL(&vg->fenceq, cmd, next);
            vg->inflight++;
        } else {
            free(cmd);
        }
    }
}