    uint32_t CiffDirectory::doWrite(Blob&     blob,
                                    ByteOrder byteOrder,
                                    uint32_t  offset)
    {
#ifdef EXIV2_DEBUG_MESSAGES
        std::cout << "Writing directory 0x" << std::hex << tag() << "---->\n";
#endif
        // Ciff offsets are relative to the start of the directory
        uint32_t dirOffset = 0;

        // Value data
        const Components::iterator b = components_.begin();
        const Components::iterator e = components_.end();
        for (Components::iterator i = b; i != e; ++i) {
            dirOffset = (*i)->write(blob, byteOrder, dirOffset);
        }
        const uint32_t dirStart = dirOffset;

        // Number of directory entries
        byte buf[4];
        us2Data(buf, static_cast<uint16_t>(components_.size()), byteOrder);
        append(blob, buf, 2);
        dirOffset += 2;

        // Directory entries
        for (Components::iterator i = b; i != e; ++i) {
            (*i)->writeDirEntry(blob, byteOrder);
            dirOffset += 10;
        }

        // Offset of directory
        ul2Data(buf, dirStart, byteOrder);
        append(blob, buf, 4);
        dirOffset += 4;

        // Update directory entry
        setOffset(offset);
        setSize(dirOffset);

#ifdef EXIV2_DEBUG_MESSAGES
        std::cout << "Directory is at offset " << std::dec << dirStart
                  << ", " << components_.size() << " entries\n"
                  << "<---- 0x" << std::hex << tag() << "\n";
#endif
        return offset + dirOffset;
    } // CiffDirectory::doWrite