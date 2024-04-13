static RELOC_PTRS_WITH(device_forward_reloc_ptrs, gx_device_forward *fdev)
{
    fdev->target = gx_device_reloc_ptr(fdev->target, gcst);
}