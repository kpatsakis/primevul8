attach_slave(ClientPtr client, xXIAttachSlaveInfo * c, int flags[MAXDEVICES])
{
    DeviceIntPtr dev;
    DeviceIntPtr newmaster;
    int rc;

    rc = dixLookupDevice(&dev, c->deviceid, client, DixManageAccess);
    if (rc != Success)
        goto unwind;

    if (IsMaster(dev)) {
        client->errorValue = c->deviceid;
        rc = BadDevice;
        goto unwind;
    }

    /* Don't allow changes to XTest Devices, these are fixed */
    if (IsXTestDevice(dev, NULL)) {
        client->errorValue = c->deviceid;
        rc = BadDevice;
        goto unwind;
    }

    rc = dixLookupDevice(&newmaster, c->new_master, client, DixAddAccess);
    if (rc != Success)
        goto unwind;
    if (!IsMaster(newmaster)) {
        client->errorValue = c->new_master;
        rc = BadDevice;
        goto unwind;
    }

    if (!((IsPointerDevice(newmaster) && IsPointerDevice(dev)) ||
          (IsKeyboardDevice(newmaster) && IsKeyboardDevice(dev)))) {
        rc = BadDevice;
        goto unwind;
    }

    ReleaseButtonsAndKeys(dev);
    AttachDevice(client, dev, newmaster);
    flags[dev->id] |= XISlaveAttached;

 unwind:
    return rc;
}