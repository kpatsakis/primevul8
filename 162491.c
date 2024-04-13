static uint64_t dp8393x_read(void *opaque, hwaddr addr, unsigned int size)
{
    dp8393xState *s = opaque;
    int reg = addr >> s->it_shift;
    uint16_t val = 0;

    switch (reg) {
        /* Update data before reading it */
        case SONIC_WT0:
        case SONIC_WT1:
            dp8393x_update_wt_regs(s);
            val = s->regs[reg];
            break;
        /* Accept read to some registers only when in reset mode */
        case SONIC_CAP2:
        case SONIC_CAP1:
        case SONIC_CAP0:
            if (s->regs[SONIC_CR] & SONIC_CR_RST) {
                val = s->cam[s->regs[SONIC_CEP] & 0xf][2* (SONIC_CAP0 - reg) + 1] << 8;
                val |= s->cam[s->regs[SONIC_CEP] & 0xf][2* (SONIC_CAP0 - reg)];
            }
            break;
        /* All other registers have no special contrainst */
        default:
            val = s->regs[reg];
    }

    DPRINTF("read 0x%04x from reg %s\n", val, reg_names[reg]);

    return s->big_endian ? val << 16 : val;
}