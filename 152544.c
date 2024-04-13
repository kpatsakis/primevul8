readNextTileData (InputStreamMutex *streamData,
                  TiledInputFile::Data *ifd,
		  int &dx, int &dy,
		  int &lx, int &ly,
                  char * & buffer,
		  int &dataSize)
{
    //
    // Read the next tile block from the file
    //

    if(isMultiPart(ifd->version))
    {
        int part;
        Xdr::read <StreamIO> (*streamData->is, part);
        if(part!=ifd->partNumber)
        {
           throw IEX_NAMESPACE::InputExc("Unexpected part number in readNextTileData");
        }
    }

    //
    // Read the first few bytes of the tile (the header).
    //

    Xdr::read <StreamIO> (*streamData->is, dx);
    Xdr::read <StreamIO> (*streamData->is, dy);
    Xdr::read <StreamIO> (*streamData->is, lx);
    Xdr::read <StreamIO> (*streamData->is, ly);
    Xdr::read <StreamIO> (*streamData->is, dataSize);

    if (dataSize > (int) ifd->tileBufferSize)
        throw IEX_NAMESPACE::InputExc ("Unexpected tile block length.");
    
    //
    // Read the pixel data.
    //

    streamData->is->read (buffer, dataSize);
    
    //
    // Keep track of which tile is the next one in
    // the file, so that we can avoid redundant seekg()
    // operations (seekg() can be fairly expensive).
    //

    streamData->currentPosition += 5 * Xdr::size<int>() + dataSize;
}