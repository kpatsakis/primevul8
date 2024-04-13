
static void intel_hda_reg_write(IntelHDAState *d, const IntelHDAReg *reg, uint32_t val,
                                uint32_t wmask)
{
    uint32_t *addr;
    uint32_t old;

    if (!reg) {
        return;
    }
    if (!reg->wmask) {
        qemu_log_mask(LOG_GUEST_ERROR, "intel-hda: write to r/o reg %s\n",
                      reg->name);
        return;
    }

    if (d->debug) {
        time_t now = time(NULL);
        if (d->last_write && d->last_reg == reg && d->last_val == val) {
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
            dprint(d, 2, "write %-16s: 0x%x (%x)\n", reg->name, val, wmask);
            d->last_write = 1;
            d->last_reg   = reg;
            d->last_val   = val;
            d->last_sec   = now;
            d->repeat_count = 0;
        }
    }
    assert(reg->offset != 0);

    addr = intel_hda_reg_addr(d, reg);
    old = *addr;

    if (reg->shift) {
        val <<= reg->shift;
        wmask <<= reg->shift;
    }
    wmask &= reg->wmask;
    *addr &= ~wmask;
    *addr |= wmask & val;
    *addr &= ~(val & reg->wclear);

    if (reg->whandler) {
        reg->whandler(d, reg, old);
    }