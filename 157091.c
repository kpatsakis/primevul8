    TEST(BSONValidate, Basic) {
        BSONObj x;
        ASSERT_TRUE( x.valid() );

        x = BSON( "x" << 1 );
        ASSERT_TRUE( x.valid() );
    }