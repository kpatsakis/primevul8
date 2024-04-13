vg_process_cursor_cmd(VuGpu *g, struct virtio_gpu_update_cursor *cursor)
{
    switch (cursor->hdr.type) {
    case VIRTIO_GPU_CMD_MOVE_CURSOR: {
        VhostUserGpuMsg msg = {
            .request = cursor->resource_id ?
                VHOST_USER_GPU_CURSOR_POS : VHOST_USER_GPU_CURSOR_POS_HIDE,
            .size = sizeof(VhostUserGpuCursorPos),
            .payload.cursor_pos = {
                .scanout_id = cursor->pos.scanout_id,
                .x = cursor->pos.x,
                .y = cursor->pos.y,
            }
        };
        g_debug("%s: move", G_STRFUNC);
        vg_send_msg(g, &msg, -1);
        break;
    }
    case VIRTIO_GPU_CMD_UPDATE_CURSOR: {
        VhostUserGpuMsg msg = {
            .request = VHOST_USER_GPU_CURSOR_UPDATE,
            .size = sizeof(VhostUserGpuCursorUpdate),
            .payload.cursor_update = {
                .pos = {
                    .scanout_id = cursor->pos.scanout_id,
                    .x = cursor->pos.x,
                    .y = cursor->pos.y,
                },
                .hot_x = cursor->hot_x,
                .hot_y = cursor->hot_y,
            }
        };
        g_debug("%s: update", G_STRFUNC);
        if (g->virgl) {
            vg_virgl_update_cursor_data(g, cursor->resource_id,
                                        msg.payload.cursor_update.data);
        } else {
            update_cursor_data_simple(g, cursor->resource_id,
                                      msg.payload.cursor_update.data);
        }
        vg_send_msg(g, &msg, -1);
        break;
    }
    default:
        g_debug("%s: unknown cmd %d", G_STRFUNC, cursor->hdr.type);
        break;
    }
}