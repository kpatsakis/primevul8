cmsBool ProcessPhotoshopAPP13(JOCTET FAR *data, int datalen)
{
    int i;

    for (i = 14; i < datalen; )
    {
        long len;
        unsigned int type;

        if (!(GETJOCTET(data[i]  ) == 0x38 &&
              GETJOCTET(data[i+1]) == 0x42 &&
              GETJOCTET(data[i+2]) == 0x49 &&
              GETJOCTET(data[i+3]) == 0x4D)) break; // Not recognized

        i += 4; // identifying string

        type = (unsigned int) (GETJOCTET(data[i]<<8) + GETJOCTET(data[i+1]));

        i += 2; // resource type

        i += GETJOCTET(data[i]) + ((GETJOCTET(data[i]) & 1) ? 1 : 2);   // resource name

        len = ((((GETJOCTET(data[i]<<8) + GETJOCTET(data[i+1]))<<8) +
                         GETJOCTET(data[i+2]))<<8) + GETJOCTET(data[i+3]);

        i += 4; // Size

        if (type == 0x03ED && len >= 16) {

            Decompressor.X_density = (UINT16) PS_FIXED_TO_FLOAT(GETJOCTET(data[i]<<8) + GETJOCTET(data[i+1]),
                                                 GETJOCTET(data[i+2]<<8) + GETJOCTET(data[i+3]));
            Decompressor.Y_density = (UINT16) PS_FIXED_TO_FLOAT(GETJOCTET(data[i+8]<<8) + GETJOCTET(data[i+9]),
                                                 GETJOCTET(data[i+10]<<8) + GETJOCTET(data[i+11]));

            // Set the density unit to 1 since the
            // Vertical and Horizontal resolutions
            // are specified in Pixels per inch

            Decompressor.density_unit = 0x01;
            return TRUE;

        }

        i += len + ((len & 1) ? 1 : 0);   // Alignment
    }
    return FALSE;
}