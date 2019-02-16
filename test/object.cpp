// Copyright (c) 2014 BitPay Inc.
// Copyright (c) 2014-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/licenses/mit-license.php.

#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <cassert>
#include <stdexcept>
#include <rlpvalue.h>

#define BOOST_FIXTURE_TEST_SUITE(a, b)
#define BOOST_AUTO_TEST_CASE(funcName) void funcName()
#define BOOST_AUTO_TEST_SUITE_END()
#define BOOST_CHECK(expr) assert(expr)
#define BOOST_CHECK_EQUAL(v1, v2) assert((v1) == (v2))
#define BOOST_CHECK_THROW(stmt, excMatch) { \
        try { \
            (stmt); \
            assert(0 && "No exception caught"); \
        } catch (excMatch & e) { \
	} catch (...) { \
	    assert(0 && "Wrong exception caught"); \
	} \
    }
#define BOOST_CHECK_NO_THROW(stmt) { \
        try { \
            (stmt); \
	} catch (...) { \
	    assert(0); \
	} \
    }

BOOST_FIXTURE_TEST_SUITE(univalue_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(univalue_constructor)
{
    RLPValue v1;
    BOOST_CHECK(v1.isNull());

    RLPValue v2(RLPValue::VSTR);
    BOOST_CHECK(v2.isStr());

    RLPValue v3(RLPValue::VSTR, "foo");
    BOOST_CHECK(v3.isStr());
    BOOST_CHECK_EQUAL(v3.getValStr(), "foo");

    RLPValue numTest;
    BOOST_CHECK(numTest.setNumStr("82"));
    BOOST_CHECK(numTest.isNum());
    BOOST_CHECK_EQUAL(numTest.getValStr(), "82");

    uint64_t vu64 = 82;
    RLPValue v4(vu64);
    BOOST_CHECK(v4.isNum());
    BOOST_CHECK_EQUAL(v4.getValStr(), "82");

    int64_t vi64 = -82;
    RLPValue v5(vi64);
    BOOST_CHECK(v5.isNum());
    BOOST_CHECK_EQUAL(v5.getValStr(), "-82");

    int vi = -688;
    RLPValue v6(vi);
    BOOST_CHECK(v6.isNum());
    BOOST_CHECK_EQUAL(v6.getValStr(), "-688");

    std::string vs("yawn");
    RLPValue v8(vs);
    BOOST_CHECK(v8.isStr());
    BOOST_CHECK_EQUAL(v8.getValStr(), "yawn");

    const char *vcs = "zappa";
    RLPValue v9(vcs);
    BOOST_CHECK(v9.isStr());
    BOOST_CHECK_EQUAL(v9.getValStr(), "zappa");
}

BOOST_AUTO_TEST_CASE(univalue_typecheck)
{
    RLPValue v1;
    BOOST_CHECK(v1.setNumStr("1"));
    BOOST_CHECK(v1.isNum());

    RLPValue v3;
    BOOST_CHECK(v3.setNumStr("32482348723847471234"));
    BOOST_CHECK_THROW(v3.get_int64(), std::runtime_error);
    BOOST_CHECK(v3.setNumStr("1000"));
    BOOST_CHECK_EQUAL(v3.get_int64(), 1000);

    RLPValue v4;
    BOOST_CHECK(v4.setNumStr("2147483648"));
    BOOST_CHECK_EQUAL(v4.get_int64(), 2147483648);
    BOOST_CHECK_THROW(v4.get_int(), std::runtime_error);
    BOOST_CHECK(v4.setNumStr("1000"));
    BOOST_CHECK_EQUAL(v4.get_int(), 1000);
    BOOST_CHECK_THROW(v4.get_str(), std::runtime_error);
    BOOST_CHECK_THROW(v4.get_array(), std::runtime_error);
    BOOST_CHECK_THROW(v4.getValues(), std::runtime_error);

    RLPValue v5;
    BOOST_CHECK(v5.read("[true, 10]"));
    BOOST_CHECK_NO_THROW(v5.get_array());
    std::vector<RLPValue> vals = v5.getValues();
    BOOST_CHECK_THROW(vals[0].get_int(), std::runtime_error);

    BOOST_CHECK_EQUAL(vals[1].get_int(), 10);
}

BOOST_AUTO_TEST_CASE(univalue_set)
{
    RLPValue v(RLPValue::VSTR, "foo");
    v.clear();
    BOOST_CHECK(v.isNull());
    BOOST_CHECK_EQUAL(v.getValStr(), "");

    BOOST_CHECK(v.setArray());
    BOOST_CHECK(v.isArray());
    BOOST_CHECK_EQUAL(v.size(), 0);

    BOOST_CHECK(v.setStr("zum"));
    BOOST_CHECK(v.isStr());
    BOOST_CHECK_EQUAL(v.getValStr(), "zum");

    BOOST_CHECK(v.setInt((int)1023));
    BOOST_CHECK(v.isNum());
    BOOST_CHECK_EQUAL(v.getValStr(), "1023");

    BOOST_CHECK(v.setInt((int64_t)-1023LL));
    BOOST_CHECK(v.isNum());
    BOOST_CHECK_EQUAL(v.getValStr(), "-1023");

    BOOST_CHECK(v.setInt((uint64_t)1023ULL));
    BOOST_CHECK(v.isNum());
    BOOST_CHECK_EQUAL(v.getValStr(), "1023");

    BOOST_CHECK(v.setNumStr("-688"));
    BOOST_CHECK(v.isNum());
    BOOST_CHECK_EQUAL(v.getValStr(), "-688");

    BOOST_CHECK(!v.setNumStr("zombocom"));

    BOOST_CHECK(v.setNull());
    BOOST_CHECK(v.isNull());
}

BOOST_AUTO_TEST_CASE(univalue_array)
{
    RLPValue arr(RLPValue::VARR);

    RLPValue v((int64_t)1023LL);
    BOOST_CHECK(arr.push_back(v));

    std::string vStr("zippy");
    BOOST_CHECK(arr.push_back(vStr));

    const char *s = "pippy";
    BOOST_CHECK(arr.push_back(s));

    std::vector<RLPValue> vec;
    v.setStr("boing");
    vec.push_back(v);

    v.setStr("going");
    vec.push_back(v);

    BOOST_CHECK(arr.push_backV(vec));

    BOOST_CHECK(arr.push_back((uint64_t) 400ULL));
    BOOST_CHECK(arr.push_back((int64_t) -400LL));
    BOOST_CHECK(arr.push_back((int) -401));

    BOOST_CHECK_EQUAL(arr.empty(), false);
    BOOST_CHECK_EQUAL(arr.size(), 9);

    BOOST_CHECK_EQUAL(arr[0].getValStr(), "1023");
    BOOST_CHECK_EQUAL(arr[1].getValStr(), "zippy");
    BOOST_CHECK_EQUAL(arr[2].getValStr(), "pippy");
    BOOST_CHECK_EQUAL(arr[3].getValStr(), "boing");
    BOOST_CHECK_EQUAL(arr[4].getValStr(), "going");
    BOOST_CHECK_EQUAL(arr[5].getValStr(), "400");
    BOOST_CHECK_EQUAL(arr[6].getValStr(), "-400");
    BOOST_CHECK_EQUAL(arr[7].getValStr(), "-401");

    BOOST_CHECK_EQUAL(arr[999].getValStr(), "");

    arr.clear();
    BOOST_CHECK(arr.empty());
    BOOST_CHECK_EQUAL(arr.size(), 0);
}

static const char *json1 =
"[1.10000000,{\"key1\":\"str\\u0000\",\"key2\":800,\"key3\":{\"name\":\"martian http://test.com\"}}]";

BOOST_AUTO_TEST_CASE(univalue_readwrite)
{
    RLPValue v;
    BOOST_CHECK(v.read(json1));

    std::string strJson1(json1);
    BOOST_CHECK(v.read(strJson1));

    BOOST_CHECK(v.isArray());
    BOOST_CHECK_EQUAL(v.size(), 2);

    BOOST_CHECK_EQUAL(v[0].getValStr(), "1.10000000");

    RLPValue obj = v[1];
    BOOST_CHECK_EQUAL(obj.size(), 3);

    BOOST_CHECK_EQUAL(strJson1, v.write());

    /* Check for (correctly reporting) a parsing error if the initial
       JSON construct is followed by more stuff.  Note that whitespace
       is, of course, exempt.  */

    BOOST_CHECK(v.read("  {}\n  "));
    BOOST_CHECK(v.read("  []\n  "));
    BOOST_CHECK(v.isArray());

    BOOST_CHECK(!v.read("@{}"));
    BOOST_CHECK(!v.read("{} garbage"));
    BOOST_CHECK(!v.read("[]{}"));
    BOOST_CHECK(!v.read("{}[]"));
    BOOST_CHECK(!v.read("{} 42"));
}

BOOST_AUTO_TEST_SUITE_END()

int main (int argc, char *argv[])
{
    univalue_constructor();
    univalue_typecheck();
    univalue_set();
    univalue_array();
    univalue_readwrite();
    return 0;
}

