//----------------------------------------------------------------------------
/// \file  test_nchar.cpp
//----------------------------------------------------------------------------
/// \brief Test cases for nchar class.
//----------------------------------------------------------------------------
// Copyright (c) 2010 Serge Aleynikov <saleyn@gmail.com>
// Created: 2010-09-30
//----------------------------------------------------------------------------
/*
***** BEGIN LICENSE BLOCK *****

This file may be included in different open-source projects

Copyright (C) 2010 Serge Aleynikov <saleyn@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

***** END LICENSE BLOCK *****
*/

#include <boost/test/unit_test.hpp>
#include <utxx/nchar.hpp>
#include <iostream>

using namespace utxx;

BOOST_AUTO_TEST_CASE( test_nchar )
{
    {
        nchar<4> rc("abcd", 4);
        std::string rcs = rc.to_string();
        BOOST_REQUIRE_EQUAL(4u, rcs.size());
        BOOST_REQUIRE_EQUAL("abcd", rcs);
    }
    {
        nchar<4> rc = std::string("ff");
        std::string rcs = rc.to_string();
        BOOST_REQUIRE_EQUAL(2u, rcs.size());
        BOOST_REQUIRE_EQUAL("ff", rcs.c_str());
    }
    {
        nchar<4> rc("ff");
        const uint8_t expect[] = {'f','f',0,0};
        BOOST_REQUIRE_EQUAL(0, memcmp(expect, (char*)rc, sizeof(expect)));
        rc = std::string("");
        std::string rcs = rc.to_string();
        BOOST_REQUIRE_EQUAL(0u, rcs.size());
        BOOST_REQUIRE_EQUAL("", rcs);
    }
    {
        const char s[] = "abcd";
        nchar<4> rc(s);
        BOOST_REQUIRE_EQUAL(rc.to_string(), "abcd");
        std::stringstream str;
        rc.dump(str);
        BOOST_REQUIRE_EQUAL(str.str(), "abcd");
    }
    {
        nchar<4> rc(1);
        BOOST_REQUIRE_EQUAL(1, rc.to_binary<int>());
        const uint8_t expect[] = {0,0,0,1};
        BOOST_REQUIRE_EQUAL(0, memcmp(expect, (char*)rc, sizeof(expect)));
        std::stringstream str;
        rc.dump(str);
        BOOST_REQUIRE_EQUAL(str.str(), "0,0,0,1");
        rc.fill(' ');
        BOOST_REQUIRE_EQUAL("    ", rc.to_string());
        BOOST_REQUIRE_EQUAL(0, rc.to_integer<int>(' '));
        rc.fill('0');
        BOOST_REQUIRE_EQUAL("0000", rc.to_string());
        BOOST_REQUIRE_EQUAL(0, rc.to_integer<int>());
    }
}

BOOST_AUTO_TEST_CASE( test_nchar_to_binary )
{
    {
        const uint8_t expect[] = {0,1};
        nchar<2> rc((uint16_t)1);
        BOOST_REQUIRE_EQUAL(1, rc.to_binary<uint16_t>());
        BOOST_REQUIRE_EQUAL(0, memcmp(expect, (char*)rc, sizeof(expect)));
    }
    {
        const uint8_t expect[] = {255, 246};
        nchar<2> rc((int16_t)-10);
        BOOST_REQUIRE_EQUAL((int16_t)-10, rc.to_binary<int16_t>());
        BOOST_REQUIRE_EQUAL(0, memcmp(expect, (char*)rc, sizeof(expect)));
    }
    {
        const uint8_t expect[] = {0,0,0,10};
        nchar<4> rc((uint32_t)10);
        BOOST_REQUIRE_EQUAL((uint32_t)10, rc.to_binary<uint32_t>());
        BOOST_REQUIRE_EQUAL(0, memcmp(expect, (char*)rc, sizeof(expect)));
    }
    {
        const uint8_t expect[] = {255, 255, 255, 246};
        nchar<4> rc((int32_t)-10);
        BOOST_REQUIRE_EQUAL((int32_t)-10, rc.to_binary<int32_t>());
        BOOST_REQUIRE_EQUAL(0, memcmp(expect, (char*)rc, sizeof(expect)));
    }
    {
        const uint8_t expect[] = {255, 255, 255, 255, 255, 255, 255, 246};
        nchar<8> rc((int64_t)-10);
        BOOST_REQUIRE_EQUAL((int64_t)-10, rc.to_binary<int64_t>());
        BOOST_REQUIRE_EQUAL(0, memcmp(expect, (char*)rc, sizeof(expect)));
    }
    {
        const uint8_t expect[] = {0, 0, 0, 0, 0, 0, 0, 10};
        nchar<8> rc((uint64_t)10);
        BOOST_REQUIRE_EQUAL((uint64_t)10, rc.to_binary<uint64_t>());
        BOOST_REQUIRE_EQUAL(0, memcmp(expect, (char*)rc, sizeof(expect)));
    }
    {
        const uint8_t expect[] = {63,240,0,0,0,0,0,0};
        nchar<8> rc(1.0);
        BOOST_REQUIRE_EQUAL(1.0, rc.to_binary<double>());
        BOOST_REQUIRE_EQUAL(0, memcmp(expect, (char*)rc, sizeof(expect)));
    }
}

BOOST_AUTO_TEST_CASE( test_nchar_to_integer )
{
    {
        nchar<8> rc1("12345678");
        BOOST_REQUIRE_EQUAL(12345678, rc1.to_integer<int>());
        BOOST_REQUIRE_EQUAL(12345678, rc1.to_integer<int>(' '));
        BOOST_REQUIRE_EQUAL(2345678,  rc1.to_integer<int>('1'));
        BOOST_REQUIRE_EQUAL(12345678, rc1.to_integer<int>('2'));
        nchar<16> rc2("-123456789012345");
        BOOST_REQUIRE_EQUAL(-123456789012345ll, rc2.to_integer<int64_t>());
        const char* p = "  12";
        nchar<4> rc3(p, 4);
        BOOST_REQUIRE_EQUAL(0, rc3.to_integer<int>());
        nchar<4> rc4(p, 4);
        BOOST_REQUIRE_EQUAL(12, rc4.to_integer<int>(' '));
        {
            const char* p = "   -";
            nchar<4> rc(p, 4);
            BOOST_REQUIRE_EQUAL(0, rc.to_integer<int>(' '));
        }
        {
            const char* p = "-123";
            nchar<4> rc(p, 4);
            BOOST_REQUIRE_EQUAL(-123, rc.to_integer<int>(' '));
            BOOST_REQUIRE_EQUAL(-123, rc.to_integer<int>('-'));
        }
    }
}

BOOST_AUTO_TEST_CASE( test_nchar_from_integer )
{
    {
        nchar<8> rc;
        rc.from_integer(12345678);
        BOOST_REQUIRE_EQUAL("12345678", rc.to_string());
    }
    {
        nchar<16> rc;
        rc.from_integer(-123456789012345);
        BOOST_REQUIRE_EQUAL("-123456789012345", rc.to_string());
    }
    {
        nchar<4> rc;
        rc.from_integer(12);
        BOOST_REQUIRE_EQUAL("12", rc.to_string());
        rc.fill(' ');
        rc.from_integer(-12);
        BOOST_REQUIRE_EQUAL("-12", rc.to_string());
        rc.fill(' ');
        rc.from_integer(12, ' ');
        BOOST_REQUIRE_EQUAL("12  ", rc.to_string());
        rc.fill(' ');
        rc.from_integer(-12, ' ');
        BOOST_REQUIRE_EQUAL("-12 ", rc.to_string());
    }
    {
        nchar<4> rc;
        rc.from_integer(0, ' ');
        BOOST_REQUIRE_EQUAL("0   ", rc.to_string());
    }
}

BOOST_AUTO_TEST_CASE( test_nchar_bad_cases )
{
    if (0)
    {
        const char* p = " -12";
        nchar<4> rc(p, 4);
        BOOST_REQUIRE_EQUAL(-12, rc.to_integer<int>(' '));
        BOOST_REQUIRE_EQUAL(0,   rc.to_integer<int>('-'));
    }
    if (0)
    {
        const char* p = "--12";
        nchar<4> rc(p, 4);
        BOOST_REQUIRE_EQUAL(0, rc.to_integer<int>('-'));
    }
    if (0)
    {
        const char* p = "  -1";
        nchar<4> rc(p, 4);
        BOOST_REQUIRE_EQUAL(-1, rc.to_integer<int>(' '));
    }
}
