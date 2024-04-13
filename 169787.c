
static uint32_t intel_hda_reg_read(IntelHDAState *d, const IntelHDAReg *reg,
                                   uint32_t rmask)
{
    uint32_t *addr, ret;

    if (!reg) {
        return 0;
    }

    if (reg->rhandler) {
        reg->rhandler(d, reg);
    }

    if (reg->offset == 0) {
        /* constant read-only register */
        ret = reg->reset;
    } else {
        addr = intel_hda_reg_addr(d, reg);
        ret = *addr;
        if (reg->shift) {
            ret >>= reg->shift;
        }
        ret &= rmask;
    }
    if (d->debug) {
        time_t now = time(NULL);
        if (!d->last_write && d->last_reg == reg && d->last_val == ret) {
            d->repeat_count++;
            if (d->last_sec != now) {
                dprint(d, 2, "previous register op repeated %d times\n", d->repeat_count);
                d->last_sec = now;
                d->repeat_count = 0;
            }
        } else {
            if (d->repeat_count) {
                dprint(d, 2, "previous register op repeated %d times\n", d->repeat_count);
            }
            dprint(d, 2, "read  %-16s: 0x%x (%x)\n", reg->name, ret, rmask);
            d->last_write = 0;
            d->last_reg   = reg;
            d->last_val   = ret;
            d->last_sec   = now;
            d->repeat_count = 0;
        }
    }
    return ret;