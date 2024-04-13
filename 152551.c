TiledInputFile::setFrameBuffer (const FrameBuffer &frameBuffer)
{
    Lock lock (*_data->_streamData);

    //
    // Set the frame buffer
    //

    //
    // Check if the new frame buffer descriptor is
    // compatible with the image file header.
    //

    const ChannelList &channels = _data->header.channels();

    for (FrameBuffer::ConstIterator j = frameBuffer.begin();
         j != frameBuffer.end();
         ++j)
    {
        ChannelList::ConstIterator i = channels.find (j.name());

        if (i == channels.end())
            continue;

        if (i.channel().xSampling != j.slice().xSampling ||
            i.channel().ySampling != j.slice().ySampling)
            THROW (IEX_NAMESPACE::ArgExc, "X and/or y subsampling factors "
				"of \"" << i.name() << "\" channel "
				"of input file \"" << fileName() << "\" are "
				"not compatible with the frame buffer's "
				"subsampling factors.");
    }

    //
    // Initialize the slice table for readPixels().
    //

    vector<TInSliceInfo> slices;
    ChannelList::ConstIterator i = channels.begin();

    for (FrameBuffer::ConstIterator j = frameBuffer.begin();
         j != frameBuffer.end();
         ++j)
    {
        while (i != channels.end() && strcmp (i.name(), j.name()) < 0)
        {
            //
            // Channel i is present in the file but not
            // in the frame buffer; data for channel i
            // will be skipped during readPixels().
            //

            slices.push_back (TInSliceInfo (i.channel().type,
					    i.channel().type,
					    0,      // base
					    0,      // xStride
					    0,      // yStride
					    false,  // fill
					    true,   // skip
					    0.0));  // fillValue
            ++i;
        }

        bool fill = false;

        if (i == channels.end() || strcmp (i.name(), j.name()) > 0)
        {
            //
            // Channel i is present in the frame buffer, but not in the file.
            // In the frame buffer, slice j will be filled with a default value.
            //

            fill = true;
        }

        slices.push_back (TInSliceInfo (j.slice().type,
                                        fill? j.slice().type: i.channel().type,
                                        j.slice().base,
                                        j.slice().xStride,
                                        j.slice().yStride,
                                        fill,
                                        false, // skip
                                        j.slice().fillValue,
                                        (j.slice().xTileCoords)? 1: 0,
                                        (j.slice().yTileCoords)? 1: 0));

        if (i != channels.end() && !fill)
            ++i;
    }

    while (i != channels.end())
    {
	//
	// Channel i is present in the file but not
	// in the frame buffer; data for channel i
	// will be skipped during readPixels().
	//

	slices.push_back (TInSliceInfo (i.channel().type,
					i.channel().type,
					0, // base
					0, // xStride
					0, // yStride
					false,  // fill
					true, // skip
					0.0)); // fillValue
	++i;
    }

    //
    // Store the new frame buffer.
    //

    _data->frameBuffer = frameBuffer;
    _data->slices = slices;
}