XISendDeviceHierarchyEvent(int flags[MAXDEVICES])
{
    xXIHierarchyEvent *ev;
    xXIHierarchyInfo *info;
    DeviceIntRec dummyDev;
    DeviceIntPtr dev;
    int i;

    if (!flags)
        return;

    ev = calloc(1, sizeof(xXIHierarchyEvent) +
                MAXDEVICES * sizeof(xXIHierarchyInfo));
    if (!ev)
        return;
    ev->type = GenericEvent;
    ev->extension = IReqCode;
    ev->evtype = XI_HierarchyChanged;
    ev->time = GetTimeInMillis();
    ev->flags = 0;
    ev->num_info = inputInfo.numDevices;

    info = (xXIHierarchyInfo *) &ev[1];
    for (dev = inputInfo.devices; dev; dev = dev->next) {
        info->deviceid = dev->id;
        info->enabled = dev->enabled;
        info->use = GetDeviceUse(dev, &info->attachment);
        info->flags = flags[dev->id];
        ev->flags |= info->flags;
        info++;
    }
    for (dev = inputInfo.off_devices; dev; dev = dev->next) {
        info->deviceid = dev->id;
        info->enabled = dev->enabled;
        info->use = GetDeviceUse(dev, &info->attachment);
        info->flags = flags[dev->id];
        ev->flags |= info->flags;
        info++;
    }

    for (i = 0; i < MAXDEVICES; i++) {
        if (flags[i] & (XIMasterRemoved | XISlaveRemoved)) {
            info->deviceid = i;
            info->enabled = FALSE;
            info->flags = flags[i];
            info->use = 0;
            ev->flags |= info->flags;
            ev->num_info++;
            info++;
        }
    }

    ev->length = bytes_to_int32(ev->num_info * sizeof(xXIHierarchyInfo));

    memset(&dummyDev, 0, sizeof(dummyDev));
    dummyDev.id = XIAllDevices;
    dummyDev.type = SLAVE;
    SendEventToAllWindows(&dummyDev, (XI_HierarchyChangedMask >> 8),
                          (xEvent *) ev, 1);
    free(ev);
}