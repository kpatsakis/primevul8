void* Type_Curve_Dup(struct _cms_typehandler_struct* self, const void *Ptr, cmsUInt32Number n)
{
    return (void*) cmsDupToneCurve((cmsToneCurve*) Ptr);

    cmsUNUSED_PARAMETER(n);
    cmsUNUSED_PARAMETER(self);
}