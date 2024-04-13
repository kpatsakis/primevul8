    WriteMethod TiffParser::encode(
              BasicIo&  io,
        const byte*     pData,
              uint32_t  size,
              ByteOrder byteOrder,
        const ExifData& exifData,
        const IptcData& iptcData,
        const XmpData&  xmpData
    )
    {
        // Copy to be able to modify the Exif data
        ExifData ed = exifData;

        // Delete IFDs which do not occur in TIFF images
        static const IfdId filteredIfds[] = {
            panaRawId
        };
        for (unsigned int i = 0; i < EXV_COUNTOF(filteredIfds); ++i) {
#ifdef DEBUG
            std::cerr << "Warning: Exif IFD " << filteredIfds[i] << " not encoded\n";
#endif
            ed.erase(std::remove_if(ed.begin(),
                                    ed.end(),
                                    FindExifdatum(filteredIfds[i])),
                     ed.end());
        }

        std::auto_ptr<TiffHeaderBase> header(new TiffHeader(byteOrder));
        return TiffParserWorker::encode(io,
                                        pData,
                                        size,
                                        ed,
                                        iptcData,
                                        xmpData,
                                        Tag::root,
                                        TiffMapping::findEncoder,
                                        header.get(),
                                        0);
    } // TiffParser::encode