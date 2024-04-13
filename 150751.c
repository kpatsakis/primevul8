static void cirrus_linear_mem_writeb(void *opaque, target_phys_addr_t addr,
                                     uint32_t val)
{
    CirrusVGAState *s = (CirrusVGAState *) opaque;

    addr &= s->cirrus_addr_mask;
    *(s->vram_ptr + addr) = val;
    cpu_physical_memory_set_dirty(s->vram_offset + addr);
}