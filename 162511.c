static void dp8393x_update_irq(dp8393xState *s)
{
    int level = (s->regs[SONIC_IMR] & s->regs[SONIC_ISR]) ? 1 : 0;

#ifdef DEBUG_SONIC
    if (level != s->irq_level) {
        s->irq_level = level;
        if (level) {
            DPRINTF("raise irq, isr is 0x%04x\n", s->regs[SONIC_ISR]);
        } else {
            DPRINTF("lower irq\n");
        }
    }
#endif

    qemu_set_irq(s->irq, level);
}