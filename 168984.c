    ExifData::const_iterator findMetadatum(const ExifData& ed,
                                           const char* keys[],
                                           int count)
    {
        for (int i = 0; i < count; ++i) {
            ExifData::const_iterator pos = ed.findKey(ExifKey(keys[i]));
            if (pos != ed.end()) return pos;
        }
        return ed.end();
    } // findMetadatum