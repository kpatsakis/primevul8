TileBufferTask::execute ()
{
    try
    {
        //
        // Calculate information about the tile
        //
    
        Box2i tileRange =  OPENEXR_IMF_INTERNAL_NAMESPACE::dataWindowForTile (
                _ifd->tileDesc,
                _ifd->minX, _ifd->maxX,
                _ifd->minY, _ifd->maxY,
                _tileBuffer->dx,
                _tileBuffer->dy,
                _tileBuffer->lx,
                _tileBuffer->ly);

        int numPixelsPerScanLine = tileRange.max.x - tileRange.min.x + 1;
    
        int numPixelsInTile = numPixelsPerScanLine *
                            (tileRange.max.y - tileRange.min.y + 1);
    
        int sizeOfTile = _ifd->bytesPerPixel * numPixelsInTile;
    
    
        //
        // Uncompress the data, if necessary
        //
    
        if (_tileBuffer->compressor && _tileBuffer->dataSize < sizeOfTile)
        {
            _tileBuffer->format = _tileBuffer->compressor->format();

            _tileBuffer->dataSize = _tileBuffer->compressor->uncompressTile
		(_tileBuffer->buffer, _tileBuffer->dataSize,
		 tileRange, _tileBuffer->uncompressedData);
        }
        else
        {
            //
            // If the line is uncompressed, it's in XDR format,
            // regardless of the compressor's output format.
            //
    
            _tileBuffer->format = Compressor::XDR;
            _tileBuffer->uncompressedData = _tileBuffer->buffer;
        }
    
        //
        // Convert the tile of pixel data back from the machine-independent
	// representation, and store the result in the frame buffer.
        //
    
        const char *readPtr = _tileBuffer->uncompressedData;
                                                        // points to where we
                                                        // read from in the
                                                        // tile block
        
        //
        // Iterate over the scan lines in the tile.
        //
    
        for (int y = tileRange.min.y; y <= tileRange.max.y; ++y)
        {
            //
            // Iterate over all image channels.
            //
            
            for (unsigned int i = 0; i < _ifd->slices.size(); ++i)
            {
                const TInSliceInfo &slice = _ifd->slices[i];
    
                //
                // These offsets are used to facilitate both
                // absolute and tile-relative pixel coordinates.
                //
            
                int xOffset = slice.xTileCoords * tileRange.min.x;
                int yOffset = slice.yTileCoords * tileRange.min.y;
    
                //
                // Fill the frame buffer with pixel data.
                //
    
                if (slice.skip)
                {
                    //
                    // The file contains data for this channel, but
                    // the frame buffer contains no slice for this channel.
                    //
    
                    skipChannel (readPtr, slice.typeInFile,
                                 numPixelsPerScanLine);
                }
                else
                {
                    //
                    // The frame buffer contains a slice for this channel.
                    //
    
                    char *writePtr = slice.base +
                                     (y - yOffset) * slice.yStride +
                                     (tileRange.min.x - xOffset) *
                                     slice.xStride;

                    char *endPtr = writePtr +
                                   (numPixelsPerScanLine - 1) * slice.xStride;
                                    
                    copyIntoFrameBuffer (readPtr, writePtr, endPtr,
                                         slice.xStride,
                                         slice.fill, slice.fillValue,
                                         _tileBuffer->format,
                                         slice.typeInFrameBuffer,
                                         slice.typeInFile);
                }
            }
        }
    }
    catch (std::exception &e)
    {
        if (!_tileBuffer->hasException)
        {
            _tileBuffer->exception = e.what ();
            _tileBuffer->hasException = true;
        }
    }
    catch (...)
    {
        if (!_tileBuffer->hasException)
        {
            _tileBuffer->exception = "unrecognized exception";
            _tileBuffer->hasException = true;
        }
    }
}