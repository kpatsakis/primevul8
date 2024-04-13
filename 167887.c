vg_panic(VuDev *dev, const char *msg)
{
    g_critical("%s\n", msg);
    exit(1);
}