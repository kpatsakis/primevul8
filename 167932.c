cd_read_sector_sync(IDEState *s)
{
    int ret;
    block_acct_start(blk_get_stats(s->blk), &s->acct,
                     ATAPI_SECTOR_SIZE, BLOCK_ACCT_READ);

    trace_cd_read_sector_sync(s->lba);

    switch (s->cd_sector_size) {
    case 2048:
        ret = blk_pread(s->blk, (int64_t)s->lba << ATAPI_SECTOR_BITS,
                        s->io_buffer, ATAPI_SECTOR_SIZE);
        break;
    case 2352:
        ret = blk_pread(s->blk, (int64_t)s->lba << ATAPI_SECTOR_BITS,
                        s->io_buffer + 16, ATAPI_SECTOR_SIZE);
        if (ret >= 0) {
            cd_data_to_raw(s->io_buffer, s->lba);
        }
        break;
    default:
        block_acct_invalid(blk_get_stats(s->blk), BLOCK_ACCT_READ);
        return -EIO;
    }

    if (ret < 0) {
        block_acct_failed(blk_get_stats(s->blk), &s->acct);
    } else {
        block_acct_done(blk_get_stats(s->blk), &s->acct);
        s->lba++;
        s->io_buffer_index = 0;
    }

    return ret;
}