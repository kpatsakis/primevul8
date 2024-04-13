gs_getdevice(int index)
{
    const gx_device *const *list;
    int count = gs_lib_device_list(&list, NULL);

    if (index < 0 || index >= count)
        return 0;		/* index out of range */
    return list[index];
}