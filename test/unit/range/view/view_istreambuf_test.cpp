// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <iostream>

#include <gtest/gtest.h>

#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/io/stream/parse_condition.hpp>
#include <seqan3/range/view/char_to.hpp>
#include <seqan3/range/view/complement.hpp>
#include <seqan3/range/view/istreambuf.hpp>
#include <seqan3/range/view/take_until.hpp>
#include <seqan3/std/ranges>

using namespace seqan3;

TEST(view_istreambuf, basic)
{
    std::string data{"ACGTATATATAT ATATAT TTA \n AUAUAA"};
    std::istringstream is{data};

    // construct from istream:
    auto v1 = view::istreambuf(is);
    size_t i = 0;
    for (auto c : v1)
        EXPECT_EQ(c, data[i++]);
    EXPECT_EQ(i, data.size());

    // construct from streambuf
    is.clear();
    is.seekg(0, std::ios::beg);
    auto v2 = view::istreambuf(*is.rdbuf());
    i = 0;
    for (auto c : v2)
        EXPECT_EQ(c, data[i++]);
    EXPECT_EQ(i, data.size());

    // combinability
    is.clear();
    is.seekg(0, std::ios::beg);
    auto v3 = view::istreambuf(is) | view::char_to<dna5> | view::complement;
    std::vector<dna5> out = v3;
    std::vector<dna5> comp{"TGCATATATATANTATATANAATNNNTATATT"_dna5};
    EXPECT_TRUE(std::ranges::equal(out, comp));

    // combinability 2
    is.clear();
    is.seekg(0, std::ios::beg);
    auto v4 = view::istreambuf(is) | view::take_until(is_space);
    std::string out2 = v4;
    std::string comp2 = "ACGTATATATAT";
    EXPECT_TRUE(std::ranges::equal(out, comp));
}

TEST(view_istreambuf, concepts)
{
    std::string data{""};
    std::istringstream is{data};
    auto v1 = view::istreambuf(is);

    EXPECT_TRUE(std::ranges::InputRange<decltype(v1)>);
    EXPECT_FALSE(std::ranges::ForwardRange<decltype(v1)>);
    EXPECT_FALSE(std::ranges::BidirectionalRange<decltype(v1)>);
    EXPECT_FALSE(std::ranges::RandomAccessRange<decltype(v1)>);
    EXPECT_TRUE(std::ranges::View<decltype(v1)>);
    EXPECT_FALSE(std::ranges::SizedRange<decltype(v1)>);
    EXPECT_FALSE(std::ranges::CommonRange<decltype(v1)>);
    EXPECT_TRUE(ConstIterableRange<decltype(v1)>);
    EXPECT_FALSE((std::ranges::OutputRange<decltype(v1), char>));
}
