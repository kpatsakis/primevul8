gx_device_retain(gx_device *dev, bool retained)
{
    int delta = (int)retained - (int)dev->retained;

    if (delta) {
        dev->retained = retained; /* do first in case dev is freed */
        rc_adjust_only(dev, delta, "gx_device_retain");
    }
}