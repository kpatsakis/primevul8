cmsTagTypeSignature DecideCurveType(cmsFloat64Number ICCVersion, const void *Data)
{
    cmsToneCurve* Curve = (cmsToneCurve*) Data;

    if (ICCVersion < 4.0) return cmsSigCurveType;
    if (Curve ->nSegments != 1) return cmsSigCurveType;          // Only 1-segment curves can be saved as parametric
    if (Curve ->Segments[0].Type < 0) return cmsSigCurveType;    // Only non-inverted curves
    if (Curve ->Segments[0].Type > 5) return cmsSigCurveType;    // Only ICC parametric curves

    return cmsSigParametricCurveType;
}