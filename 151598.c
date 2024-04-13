void *Type_ParametricCurve_Read(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsUInt32Number* nItems, cmsUInt32Number SizeOfTag)
{
    static const int ParamsByType[] = { 1, 3, 4, 5, 7 };
    cmsFloat64Number Params[10];
    cmsUInt16Number Type;
    int i, n;
    cmsToneCurve* NewGamma;

    if (!_cmsReadUInt16Number(io, &Type)) return NULL;
    if (!_cmsReadUInt16Number(io, NULL)) return NULL;   // Reserved

    if (Type > 4) {

        cmsSignalError(self->ContextID, cmsERROR_UNKNOWN_EXTENSION, "Unknown parametric curve type '%d'", Type);
        return NULL;
    }

    memset(Params, 0, sizeof(Params));
    n = ParamsByType[Type];

    for (i=0; i < n; i++) {

        if (!_cmsRead15Fixed16Number(io, &Params[i])) return NULL;
    }

    NewGamma = cmsBuildParametricToneCurve(self ->ContextID, Type+1, Params);

    *nItems = 1;
    return NewGamma;

    cmsUNUSED_PARAMETER(SizeOfTag);
}