cmsBool WriteMPECurve(struct _cms_typehandler_struct* self,
                      cmsIOHANDLER* io,
                      void* Cargo,
                      cmsUInt32Number n,
                      cmsUInt32Number SizeOfTag)
{
    _cmsStageToneCurvesData* Curves  = (_cmsStageToneCurvesData*) Cargo;

    return WriteSegmentedCurve(io, Curves ->TheCurves[n]);

    cmsUNUSED_PARAMETER(SizeOfTag);
    cmsUNUSED_PARAMETER(self);
}