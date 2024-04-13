static ssize_t dp8393x_receive(NetClientState *nc, const uint8_t * buf,
                               size_t pkt_size)
{
    dp8393xState *s = qemu_get_nic_opaque(nc);
    int packet_type;
    uint32_t available, address;
    int width, rx_len, padded_len;
    uint32_t checksum;
    int size;

    s->regs[SONIC_RCR] &= ~(SONIC_RCR_PRX | SONIC_RCR_LBK | SONIC_RCR_FAER |
        SONIC_RCR_CRCR | SONIC_RCR_LPKT | SONIC_RCR_BC | SONIC_RCR_MC);

    if (s->last_rba_is_full) {
        return pkt_size;
    }

    rx_len = pkt_size + sizeof(checksum);
    if (s->regs[SONIC_DCR] & SONIC_DCR_DW) {
        width = 2;
        padded_len = ((rx_len - 1) | 3) + 1;
    } else {
        width = 1;
        padded_len = ((rx_len - 1) | 1) + 1;
    }

    if (padded_len > dp8393x_rbwc(s) * 2) {
        DPRINTF("oversize packet, pkt_size is %d\n", pkt_size);
        s->regs[SONIC_ISR] |= SONIC_ISR_RBAE;
        dp8393x_update_irq(s);
        s->regs[SONIC_RCR] |= SONIC_RCR_LPKT;
        goto done;
    }

    packet_type = dp8393x_receive_filter(s, buf, pkt_size);
    if (packet_type < 0) {
        DPRINTF("packet not for netcard\n");
        return -1;
    }

    /* Check for EOL */
    if (s->regs[SONIC_LLFA] & SONIC_DESC_EOL) {
        /* Are we still in resource exhaustion? */
        size = sizeof(uint16_t) * 1 * width;
        address = dp8393x_crda(s) + sizeof(uint16_t) * 5 * width;
        address_space_read(&s->as, address, MEMTXATTRS_UNSPECIFIED,
                           s->data, size);
        s->regs[SONIC_LLFA] = dp8393x_get(s, width, 0);
        if (s->regs[SONIC_LLFA] & SONIC_DESC_EOL) {
            /* Still EOL ; stop reception */
            return -1;
        }
        /* Link has been updated by host */

        /* Clear in_use */
        size = sizeof(uint16_t) * width;
        address = dp8393x_crda(s) + sizeof(uint16_t) * 6 * width;
        dp8393x_put(s, width, 0, 0);
        address_space_rw(&s->as, address, MEMTXATTRS_UNSPECIFIED,
                         (uint8_t *)s->data, size, 1);

        /* Move to next descriptor */
        s->regs[SONIC_CRDA] = s->regs[SONIC_LLFA];
        s->regs[SONIC_ISR] |= SONIC_ISR_PKTRX;
    }

    /* Save current position */
    s->regs[SONIC_TRBA1] = s->regs[SONIC_CRBA1];
    s->regs[SONIC_TRBA0] = s->regs[SONIC_CRBA0];

    /* Calculate the ethernet checksum */
    checksum = cpu_to_le32(crc32(0, buf, pkt_size));

    /* Put packet into RBA */
    DPRINTF("Receive packet at %08x\n", dp8393x_crba(s));
    address = dp8393x_crba(s);
    address_space_write(&s->as, address, MEMTXATTRS_UNSPECIFIED,
                        buf, pkt_size);
    address += pkt_size;

    /* Put frame checksum into RBA */
    address_space_write(&s->as, address, MEMTXATTRS_UNSPECIFIED,
                        &checksum, sizeof(checksum));
    address += sizeof(checksum);

    /* Pad short packets to keep pointers aligned */
    if (rx_len < padded_len) {
        size = padded_len - rx_len;
        address_space_rw(&s->as, address, MEMTXATTRS_UNSPECIFIED,
            (uint8_t *)"\xFF\xFF\xFF", size, 1);
        address += size;
    }

    s->regs[SONIC_CRBA1] = address >> 16;
    s->regs[SONIC_CRBA0] = address & 0xffff;
    available = dp8393x_rbwc(s);
    available -= padded_len >> 1;
    s->regs[SONIC_RBWC1] = available >> 16;
    s->regs[SONIC_RBWC0] = available & 0xffff;

    /* Update status */
    if (dp8393x_rbwc(s) < s->regs[SONIC_EOBC]) {
        s->regs[SONIC_RCR] |= SONIC_RCR_LPKT;
    }
    s->regs[SONIC_RCR] |= packet_type;
    s->regs[SONIC_RCR] |= SONIC_RCR_PRX;
    if (s->loopback_packet) {
        s->regs[SONIC_RCR] |= SONIC_RCR_LBK;
        s->loopback_packet = 0;
    }

    /* Write status to memory */
    DPRINTF("Write status at %08x\n", dp8393x_crda(s));
    dp8393x_put(s, width, 0, s->regs[SONIC_RCR]); /* status */
    dp8393x_put(s, width, 1, rx_len); /* byte count */
    dp8393x_put(s, width, 2, s->regs[SONIC_TRBA0]); /* pkt_ptr0 */
    dp8393x_put(s, width, 3, s->regs[SONIC_TRBA1]); /* pkt_ptr1 */
    dp8393x_put(s, width, 4, s->regs[SONIC_RSC]); /* seq_no */
    size = sizeof(uint16_t) * 5 * width;
    address_space_write(&s->as, dp8393x_crda(s),
                        MEMTXATTRS_UNSPECIFIED,
                        s->data, size);

    /* Check link field */
    size = sizeof(uint16_t) * width;
    address_space_read(&s->as,
                       dp8393x_crda(s) + sizeof(uint16_t) * 5 * width,
                       MEMTXATTRS_UNSPECIFIED, s->data, size);
    s->regs[SONIC_LLFA] = dp8393x_get(s, width, 0);
    if (s->regs[SONIC_LLFA] & SONIC_DESC_EOL) {
        /* EOL detected */
        s->regs[SONIC_ISR] |= SONIC_ISR_RDE;
    } else {
        /* Clear in_use */
        size = sizeof(uint16_t) * width;
        address = dp8393x_crda(s) + sizeof(uint16_t) * 6 * width;
        dp8393x_put(s, width, 0, 0);
        address_space_write(&s->as, address, MEMTXATTRS_UNSPECIFIED,
                            s->data, size);

        /* Move to next descriptor */
        s->regs[SONIC_CRDA] = s->regs[SONIC_LLFA];
        s->regs[SONIC_ISR] |= SONIC_ISR_PKTRX;
    }

    dp8393x_update_irq(s);

    s->regs[SONIC_RSC] = (s->regs[SONIC_RSC] & 0xff00) |
                         ((s->regs[SONIC_RSC] + 1) & 0x00ff);

done:

    if (s->regs[SONIC_RCR] & SONIC_RCR_LPKT) {
        if (s->regs[SONIC_RRP] == s->regs[SONIC_RWP]) {
            /* Stop packet reception */
            s->last_rba_is_full = true;
        } else {
            /* Read next resource */
            dp8393x_do_read_rra(s);
        }
    }

    return pkt_size;
}