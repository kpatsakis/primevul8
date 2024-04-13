void* Type_Measurement_Dup(struct _cms_typehandler_struct* self, const void *Ptr, cmsUInt32Number n)
{
     return _cmsDupMem(self ->ContextID, Ptr, sizeof(cmsICCMeasurementConditions));

     cmsUNUSED_PARAMETER(n);
}