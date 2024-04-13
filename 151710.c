cmsBool  Type_Curve_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsToneCurve* Curve = (cmsToneCurve*) Ptr;

    if (Curve ->nSegments == 1 && Curve ->Segments[0].Type == 1) {

            // Single gamma, preserve number
            cmsUInt16Number SingleGammaFixed = _cmsDoubleTo8Fixed8(Curve ->Segments[0].Params[0]);

            if (!_cmsWriteUInt32Number(io, 1)) return FALSE;
            if (!_cmsWriteUInt16Number(io, SingleGammaFixed)) return FALSE;
            return TRUE;

    }

    if (!_cmsWriteUInt32Number(io, Curve ->nEntries)) return FALSE;
    return _cmsWriteUInt16Array(io, Curve ->nEntries, Curve ->Table16);

    cmsUNUSED_PARAMETER(nItems);
    cmsUNUSED_PARAMETER(self);
}