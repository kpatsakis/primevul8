remove_master(ClientPtr client, xXIRemoveMasterInfo * r, int flags[MAXDEVICES])
{
    DeviceIntPtr ptr, keybd, XTestptr, XTestkeybd;
    int rc = Success;

    if (r->return_mode != XIAttachToMaster && r->return_mode != XIFloating)
        return BadValue;

    rc = dixLookupDevice(&ptr, r->deviceid, client, DixDestroyAccess);
    if (rc != Success)
        goto unwind;

    if (!IsMaster(ptr)) {
        client->errorValue = r->deviceid;
        rc = BadDevice;
        goto unwind;
    }

    /* XXX: For now, don't allow removal of VCP, VCK */
    if (ptr == inputInfo.pointer ||ptr == inputInfo.keyboard) {
        rc = BadDevice;
        goto unwind;
    }

    ptr = GetMaster(ptr, MASTER_POINTER);
    rc = dixLookupDevice(&ptr, ptr->id, client, DixDestroyAccess);
    if (rc != Success)
        goto unwind;
    keybd = GetMaster(ptr, MASTER_KEYBOARD);
    rc = dixLookupDevice(&keybd, keybd->id, client, DixDestroyAccess);
    if (rc != Success)
        goto unwind;

    XTestptr = GetXTestDevice(ptr);
    rc = dixLookupDevice(&XTestptr, XTestptr->id, client, DixDestroyAccess);
    if (rc != Success)
        goto unwind;

    XTestkeybd = GetXTestDevice(keybd);
    rc = dixLookupDevice(&XTestkeybd, XTestkeybd->id, client, DixDestroyAccess);
    if (rc != Success)
        goto unwind;

    disable_clientpointer(ptr);

    /* Disabling sends the devices floating, reattach them if
     * desired. */
    if (r->return_mode == XIAttachToMaster) {
        DeviceIntPtr attached, newptr, newkeybd;

        rc = dixLookupDevice(&newptr, r->return_pointer, client, DixAddAccess);
        if (rc != Success)
            goto unwind;

        if (!IsMaster(newptr)) {
            client->errorValue = r->return_pointer;
            rc = BadDevice;
            goto unwind;
        }

        rc = dixLookupDevice(&newkeybd, r->return_keyboard,
                             client, DixAddAccess);
        if (rc != Success)
            goto unwind;

        if (!IsMaster(newkeybd)) {
            client->errorValue = r->return_keyboard;
            rc = BadDevice;
            goto unwind;
        }

        for (attached = inputInfo.devices; attached; attached = attached->next) {
            if (!IsMaster(attached)) {
                if (GetMaster(attached, MASTER_ATTACHED) == ptr) {
                    AttachDevice(client, attached, newptr);
                    flags[attached->id] |= XISlaveAttached;
                }
                if (GetMaster(attached, MASTER_ATTACHED) == keybd) {
                    AttachDevice(client, attached, newkeybd);
                    flags[attached->id] |= XISlaveAttached;
                }
            }
        }
    }

    for (int i = 0; i < currentMaxClients; i++)
        XIBarrierRemoveMasterDevice(clients[i], ptr->id);

    /* disable the remove the devices, XTest devices must be done first
       else the sprites they rely on will be destroyed  */
    DisableDevice(XTestptr, FALSE);
    DisableDevice(XTestkeybd, FALSE);
    DisableDevice(keybd, FALSE);
    DisableDevice(ptr, FALSE);
    flags[XTestptr->id] |= XIDeviceDisabled | XISlaveDetached;
    flags[XTestkeybd->id] |= XIDeviceDisabled | XISlaveDetached;
    flags[keybd->id] |= XIDeviceDisabled;
    flags[ptr->id] |= XIDeviceDisabled;

    flags[XTestptr->id] |= XISlaveRemoved;
    flags[XTestkeybd->id] |= XISlaveRemoved;
    flags[keybd->id] |= XIMasterRemoved;
    flags[ptr->id] |= XIMasterRemoved;

    RemoveDevice(XTestptr, FALSE);
    RemoveDevice(XTestkeybd, FALSE);
    RemoveDevice(keybd, FALSE);
    RemoveDevice(ptr, FALSE);

 unwind:
    return rc;
}