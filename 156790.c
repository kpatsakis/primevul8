static void scsi_unmap_complete_noio(UnmapCBData *data, int ret)
{
    SCSIDiskReq *r = data->r;
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, r->req.dev);

    assert(r->req.aiocb == NULL);

    if (data->count > 0) {
        uint64_t sector_num = ldq_be_p(&data->inbuf[0]);
        uint32_t nb_sectors = ldl_be_p(&data->inbuf[8]) & 0xffffffffULL;
        r->sector = sector_num * (s->qdev.blocksize / BDRV_SECTOR_SIZE);
        r->sector_count = nb_sectors * (s->qdev.blocksize / BDRV_SECTOR_SIZE);

        if (!check_lba_range(s, sector_num, nb_sectors)) {
            block_acct_invalid(blk_get_stats(s->qdev.conf.blk),
                               BLOCK_ACCT_UNMAP);
            scsi_check_condition(r, SENSE_CODE(LBA_OUT_OF_RANGE));
            goto done;
        }

        block_acct_start(blk_get_stats(s->qdev.conf.blk), &r->acct,
                         r->sector_count * BDRV_SECTOR_SIZE,
                         BLOCK_ACCT_UNMAP);

        r->req.aiocb = blk_aio_pdiscard(s->qdev.conf.blk,
                                        r->sector * BDRV_SECTOR_SIZE,
                                        r->sector_count * BDRV_SECTOR_SIZE,
                                        scsi_unmap_complete, data);
        data->count--;
        data->inbuf += 16;
        return;
    }

    scsi_req_complete(&r->req, GOOD);

done:
    scsi_req_unref(&r->req);
    g_free(data);
}