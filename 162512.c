static void dp8393x_instance_init(Object *obj)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    dp8393xState *s = DP8393X(obj);

    sysbus_init_mmio(sbd, &s->mmio);
    sysbus_init_mmio(sbd, &s->prom);
    sysbus_init_irq(sbd, &s->irq);
}