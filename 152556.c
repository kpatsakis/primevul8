readTileData (InputStreamMutex *streamData,
              TiledInputFile::Data *ifd,
	      int dx, int dy,
	      int lx, int ly,
              char *&buffer,
              int &dataSize)
{
    //
    // Read a single tile block from the file and into the array pointed
    // to by buffer.  If the file is memory-mapped, then we change where
    // buffer points instead of writing into the array (hence buffer needs
    // to be a reference to a char *).
    //

    //
    // Look up the location for this tile in the Index and
    // seek to that position if necessary
    //
    
    Int64 tileOffset = ifd->tileOffsets (dx, dy, lx, ly);

    if (tileOffset == 0)
    {
        THROW (IEX_NAMESPACE::InputExc, "Tile (" << dx << ", " << dy << ", " <<
			      lx << ", " << ly << ") is missing.");
    }


    //
    // In a multi-part file, the next chunk does not need to
    // belong to the same part, so we have to compare the
    // offset here.
    //

    if (!isMultiPart(ifd->version))
    {
        if (streamData->currentPosition != tileOffset)
            streamData->is->seekg (tileOffset);
    }
    else
    {
        //
        // In a multi-part file, the file pointer may be moved by other
        // parts, so we have to ask tellg() where we are.
        //
        if (streamData->is->tellg() != tileOffset)
            streamData->is->seekg (tileOffset);
    }

    //
    // Read the first few bytes of the tile (the header).
    // Verify that the tile coordinates and the level number
    // are correct.
    //
    
    int tileXCoord, tileYCoord, levelX, levelY;

    if (isMultiPart(ifd->version))
    {
        int partNumber;
        Xdr::read <StreamIO> (*streamData->is, partNumber);
        if (partNumber != ifd->partNumber)
        {
            THROW (IEX_NAMESPACE::ArgExc, "Unexpected part number " << partNumber
                   << ", should be " << ifd->partNumber << ".");
        }
    }

    OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*streamData->is, tileXCoord);
    OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*streamData->is, tileYCoord);
    OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*streamData->is, levelX);
    OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*streamData->is, levelY);
    OPENEXR_IMF_INTERNAL_NAMESPACE::Xdr::read <OPENEXR_IMF_INTERNAL_NAMESPACE::StreamIO> (*streamData->is, dataSize);

    if (tileXCoord != dx)
        throw IEX_NAMESPACE::InputExc ("Unexpected tile x coordinate.");

    if (tileYCoord != dy)
        throw IEX_NAMESPACE::InputExc ("Unexpected tile y coordinate.");

    if (levelX != lx)
        throw IEX_NAMESPACE::InputExc ("Unexpected tile x level number coordinate.");

    if (levelY != ly)
        throw IEX_NAMESPACE::InputExc ("Unexpected tile y level number coordinate.");

    if (dataSize < 0 || dataSize > static_cast<int>(ifd->tileBufferSize) )
        throw IEX_NAMESPACE::InputExc ("Unexpected tile block length.");

    //
    // Read the pixel data.
    //

    if (streamData->is->isMemoryMapped ())
        buffer = streamData->is->readMemoryMapped (dataSize);
    else
        streamData->is->read (buffer, dataSize);

    //
    // Keep track of which tile is the next one in
    // the file, so that we can avoid redundant seekg()
    // operations (seekg() can be fairly expensive).
    //
    
    streamData->currentPosition = tileOffset + 5 * Xdr::size<int>() + dataSize;
}