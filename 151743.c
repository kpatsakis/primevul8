cmsBool PreOptimize(cmsPipeline* Lut)
{
    cmsBool AnyOpt = FALSE, Opt;

    do {

        Opt = FALSE;

        // Remove all identities
        Opt |= _Remove1Op(Lut, cmsSigIdentityElemType);

        // Remove XYZ2Lab followed by Lab2XYZ
        Opt |= _Remove2Op(Lut, cmsSigXYZ2LabElemType, cmsSigLab2XYZElemType);

        // Remove Lab2XYZ followed by XYZ2Lab
        Opt |= _Remove2Op(Lut, cmsSigLab2XYZElemType, cmsSigXYZ2LabElemType);

        // Remove V4 to V2 followed by V2 to V4
        Opt |= _Remove2Op(Lut, cmsSigLabV4toV2, cmsSigLabV2toV4);

        // Remove V2 to V4 followed by V4 to V2
        Opt |= _Remove2Op(Lut, cmsSigLabV2toV4, cmsSigLabV4toV2);

        // Remove float pcs Lab conversions
        Opt |= _Remove2Op(Lut, cmsSigLab2FloatPCS, cmsSigFloatPCS2Lab);

        // Remove float pcs Lab conversions
        Opt |= _Remove2Op(Lut, cmsSigXYZ2FloatPCS, cmsSigFloatPCS2XYZ);

        // Simplify matrix. 
        Opt |= _MultiplyMatrix(Lut);

        if (Opt) AnyOpt = TRUE;

    } while (Opt);

    return AnyOpt;
}