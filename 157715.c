    TiffImage::TiffImage(BasicIo::AutoPtr io, bool /*create*/)
        : Image(ImageType::tiff, mdExif | mdIptc | mdXmp, io),
          pixelWidth_(0), pixelHeight_(0)
    {
    } // TiffImage::TiffImage