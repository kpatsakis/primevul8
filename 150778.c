static void cirrus_vga_save(QEMUFile *f, void *opaque)
{
    CirrusVGAState *s = opaque;

    if (s->pci_dev)
        pci_device_save(s->pci_dev, f);

    qemu_put_be32s(f, &s->latch);
    qemu_put_8s(f, &s->sr_index);
    qemu_put_buffer(f, s->sr, 256);
    qemu_put_8s(f, &s->gr_index);
    qemu_put_8s(f, &s->cirrus_shadow_gr0);
    qemu_put_8s(f, &s->cirrus_shadow_gr1);
    qemu_put_buffer(f, s->gr + 2, 254);
    qemu_put_8s(f, &s->ar_index);
    qemu_put_buffer(f, s->ar, 21);
    qemu_put_be32(f, s->ar_flip_flop);
    qemu_put_8s(f, &s->cr_index);
    qemu_put_buffer(f, s->cr, 256);
    qemu_put_8s(f, &s->msr);
    qemu_put_8s(f, &s->fcr);
    qemu_put_8s(f, &s->st00);
    qemu_put_8s(f, &s->st01);

    qemu_put_8s(f, &s->dac_state);
    qemu_put_8s(f, &s->dac_sub_index);
    qemu_put_8s(f, &s->dac_read_index);
    qemu_put_8s(f, &s->dac_write_index);
    qemu_put_buffer(f, s->dac_cache, 3);
    qemu_put_buffer(f, s->palette, 768);

    qemu_put_be32(f, s->bank_offset);

    qemu_put_8s(f, &s->cirrus_hidden_dac_lockindex);
    qemu_put_8s(f, &s->cirrus_hidden_dac_data);

    qemu_put_be32s(f, &s->hw_cursor_x);
    qemu_put_be32s(f, &s->hw_cursor_y);
    /* XXX: we do not save the bitblt state - we assume we do not save
       the state when the blitter is active */
}