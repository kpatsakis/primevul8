            bool readNumber( N* out ) {
                if ( ( _position + sizeof(N) ) > _maxLength )
                    return false;
                if ( out ) {
                    *out = ConstDataView(_buffer).readLE<N>(_position);
                }
                _position += sizeof(N);
                return true;
            }