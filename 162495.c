static void dp8393x_reset(DeviceState *dev)
{
    dp8393xState *s = DP8393X(dev);
    timer_del(s->watchdog);

    memset(s->regs, 0, sizeof(s->regs));
    s->regs[SONIC_SR] = 0x0004; /* only revision recognized by Linux/mips */
    s->regs[SONIC_CR] = SONIC_CR_RST | SONIC_CR_STP | SONIC_CR_RXDIS;
    s->regs[SONIC_DCR] &= ~(SONIC_DCR_EXBUS | SONIC_DCR_LBR);
    s->regs[SONIC_RCR] &= ~(SONIC_RCR_LB0 | SONIC_RCR_LB1 | SONIC_RCR_BRD | SONIC_RCR_RNT);
    s->regs[SONIC_TCR] |= SONIC_TCR_NCRS | SONIC_TCR_PTX;
    s->regs[SONIC_TCR] &= ~SONIC_TCR_BCM;
    s->regs[SONIC_IMR] = 0;
    s->regs[SONIC_ISR] = 0;
    s->regs[SONIC_DCR2] = 0;
    s->regs[SONIC_EOBC] = 0x02F8;
    s->regs[SONIC_RSC] = 0;
    s->regs[SONIC_CE] = 0;
    s->regs[SONIC_RSC] = 0;

    /* Network cable is connected */
    s->regs[SONIC_RCR] |= SONIC_RCR_CRS;

    dp8393x_update_irq(s);
}