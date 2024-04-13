newTileBufferTask
    (TaskGroup *group,
     InputStreamMutex *streamData,
     TiledInputFile::Data *ifd,
     int number,
     int dx, int dy,
     int lx, int ly)
{
    //
    // Wait for a tile buffer to become available,
    // fill the buffer with raw data from the file,
    // and create a new TileBufferTask whose execute()
    // method will uncompress the tile and copy the
    // tile's pixels into the frame buffer.
    //

    TileBuffer *tileBuffer = ifd->getTileBuffer (number);

    try
    {
	tileBuffer->wait();
	
	tileBuffer->dx = dx;
	tileBuffer->dy = dy;
	tileBuffer->lx = lx;
	tileBuffer->ly = ly;

	tileBuffer->uncompressedData = 0;

	readTileData (streamData, ifd, dx, dy, lx, ly,
		      tileBuffer->buffer,
		      tileBuffer->dataSize);
    }
    catch (...)
    {
	//
	// Reading from the file caused an exception.
	// Signal that the tile buffer is free, and
	// re-throw the exception.
	//

	tileBuffer->post();
	throw;
    }

    return new TileBufferTask (group, ifd, tileBuffer);
}