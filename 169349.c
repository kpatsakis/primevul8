    Image::Image(int              imageType,
                 uint16_t         supportedMetadata,
                 BasicIo::AutoPtr io)
        : io_(io),
          pixelWidth_(0),
          pixelHeight_(0),
          imageType_(imageType),
          supportedMetadata_(supportedMetadata),
#ifdef EXV_HAVE_XMP_TOOLKIT
          writeXmpFromPacket_(false),
#else
          writeXmpFromPacket_(true),
#endif
          byteOrder_(invalidByteOrder),
          tags_(),
          init_(true)
    {
    }