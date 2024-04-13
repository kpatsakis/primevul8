cmsUInt32Number GetInputPixelType(void)
{
     int space, bps, extra, ColorChannels, Flavor;

     lIsITUFax         = IsITUFax(Decompressor.marker_list);
     lIsPhotoshopApp13 = HandlePhotoshopAPP13(Decompressor.marker_list);
     lIsEXIF           = HandleEXIF(&Decompressor);

     ColorChannels = Decompressor.num_components;
     extra  = 0;            // Alpha = None
     bps    = 1;            // 8 bits
     Flavor = 0;            // Vanilla

     if (lIsITUFax) {

        space = PT_Lab;
        Decompressor.out_color_space = JCS_YCbCr;  // Fake to don't touch
     }
     else
     switch (Decompressor.jpeg_color_space) {

     case JCS_GRAYSCALE:        // monochrome
              space = PT_GRAY;
              Decompressor.out_color_space = JCS_GRAYSCALE;
              break;

     case JCS_RGB:             // red/green/blue
              space = PT_RGB;
              Decompressor.out_color_space = JCS_RGB;
              break;

     case JCS_YCbCr:               // Y/Cb/Cr (also known as YUV)
              space = PT_RGB;      // Let IJG code to do the conversion
              Decompressor.out_color_space = JCS_RGB;
              break;

     case JCS_CMYK:            // C/M/Y/K
              space = PT_CMYK;
              Decompressor.out_color_space = JCS_CMYK;
              if (Decompressor.saw_Adobe_marker)            // Adobe keeps CMYK inverted, so change flavor
                                Flavor = 1;                 // from vanilla to chocolate
              break;

     case JCS_YCCK:            // Y/Cb/Cr/K
              space = PT_CMYK;
              Decompressor.out_color_space = JCS_CMYK;
              if (Decompressor.saw_Adobe_marker)            // ditto
                                Flavor = 1;
              break;

     default:
              FatalError("Unsupported color space (0x%x)", Decompressor.jpeg_color_space);
              return 0;
     }

     return (EXTRA_SH(extra)|CHANNELS_SH(ColorChannels)|BYTES_SH(bps)|COLORSPACE_SH(space)|FLAVOR_SH(Flavor));
}