    Image::AutoPtr ImageFactory::create(int type,
                                        BasicIo::AutoPtr io)
    {
        // BasicIo instance does not need to be open
        const Registry* r = find(registry, type);
        if (0 != r) {
            return r->newInstance_(io, true);
        }
        return Image::AutoPtr();
    } // ImageFactory::create