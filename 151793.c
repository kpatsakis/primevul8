void* Type_LUT8_Dup(struct _cms_typehandler_struct* self, const void *Ptr, cmsUInt32Number n)
{
    return (void*) cmsPipelineDup((cmsPipeline*) Ptr);

    cmsUNUSED_PARAMETER(n);
    cmsUNUSED_PARAMETER(self);
}