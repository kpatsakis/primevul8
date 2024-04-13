    void CiffDirectory::readDirectory(const byte* pData,
                                      uint32_t    size,
                                      ByteOrder   byteOrder)
    {
        if (size < 4)
            throw Error(kerCorruptedMetadata);
        uint32_t o = getULong(pData + size - 4, byteOrder);
        if ( o > size-2 )
            throw Error(kerCorruptedMetadata);
        uint16_t count = getUShort(pData + o, byteOrder);
#ifdef EXIV2_DEBUG_MESSAGES
        std::cout << "Directory at offset " << std::dec << o
                  <<", " << count << " entries \n";
#endif
        o += 2;
        if ( static_cast<uint32_t>(count) * 10 > size-o )
            throw Error(kerCorruptedMetadata);

        for (uint16_t i = 0; i < count; ++i) {
            uint16_t tag = getUShort(pData + o, byteOrder);
            CiffComponent::AutoPtr m;
            switch (CiffComponent::typeId(tag)) {
            case directory: m = CiffComponent::AutoPtr(new CiffDirectory); break;
            default: m = CiffComponent::AutoPtr(new CiffEntry); break;
            }
            m->setDir(this->tag());
            m->read(pData, size, o, byteOrder);
            add(m);
            o += 10;
        }
    }  // CiffDirectory::readDirectory