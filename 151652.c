cmsBool  Type_ParametricCurve_Write(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, void* Ptr, cmsUInt32Number nItems)
{
    cmsToneCurve* Curve = (cmsToneCurve*) Ptr;
    int i, nParams, typen;
    static const int ParamsByType[] = { 0, 1, 3, 4, 5, 7 };

    typen = Curve -> Segments[0].Type;

    if (Curve ->nSegments > 1 || typen < 1) {

        cmsSignalError(self->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Multisegment or Inverted parametric curves cannot be written");
        return FALSE;
    }

    if (typen > 5) {
        cmsSignalError(self->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unsupported parametric curve");
        return FALSE;
    }

    nParams = ParamsByType[typen];

    if (!_cmsWriteUInt16Number(io, (cmsUInt16Number) (Curve ->Segments[0].Type - 1))) return FALSE;
    if (!_cmsWriteUInt16Number(io, 0)) return FALSE;        // Reserved

    for (i=0; i < nParams; i++) {

        if (!_cmsWrite15Fixed16Number(io, Curve -> Segments[0].Params[i])) return FALSE;
    }

    return TRUE;

    cmsUNUSED_PARAMETER(nItems);
}