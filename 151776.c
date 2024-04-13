cmsBool  Type_Measurement_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsICCMeasurementConditions* mc =(cmsICCMeasurementConditions*) Ptr;

    if (!_cmsWriteUInt32Number(io, mc->Observer)) return FALSE;
    if (!_cmsWriteXYZNumber(io,    &mc->Backing)) return FALSE;
    if (!_cmsWriteUInt32Number(io, mc->Geometry)) return FALSE;
    if (!_cmsWrite15Fixed16Number(io, mc->Flare)) return FALSE;
    if (!_cmsWriteUInt32Number(io, mc->IlluminantType)) return FALSE;

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}