// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

#include <deque>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <range/v3/view/unique.hpp>

#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/range/view/drop.hpp>
#include <seqan3/range/view/single_pass_input.hpp>
#include <seqan3/range/concept.hpp>
#include <seqan3/range/container/concept.hpp>
#include <seqan3/range/view/to_char.hpp>
#include <seqan3/std/concepts>
#include <seqan3/std/ranges>

using namespace seqan3;

// ============================================================================
//  test templates
// ============================================================================

template <typename adaptor_t>
void do_test(adaptor_t const & adaptor, std::string const & vec)
{
    // pipe notation
    auto v = vec | adaptor(3);
    EXPECT_EQ("bar", std::string(v));

    // function notation
    std::string v2{adaptor(vec, 3)};
    EXPECT_EQ("bar", v2);

    // combinability
    auto v3 = vec | adaptor(1) | adaptor(1) | ranges::view::unique;
    EXPECT_EQ("obar", std::string(v3));
    std::string v3b = vec | std::view::reverse | adaptor(3) | ranges::view::unique;
    EXPECT_EQ("of", v3b);
}

template <typename adaptor_t>
void do_concepts(adaptor_t && adaptor)
{
    std::vector vec{1, 2, 3};
    EXPECT_TRUE(std::ranges::InputRange<decltype(vec)>);
    EXPECT_TRUE(std::ranges::ForwardRange<decltype(vec)>);
    EXPECT_TRUE(std::ranges::BidirectionalRange<decltype(vec)>);
    EXPECT_TRUE(std::ranges::RandomAccessRange<decltype(vec)>);
    EXPECT_FALSE(std::ranges::View<decltype(vec)>);
    EXPECT_TRUE(std::ranges::SizedRange<decltype(vec)>);
    EXPECT_TRUE(std::ranges::CommonRange<decltype(vec)>);
    EXPECT_TRUE(const_iterable_concept<decltype(vec)>);
    EXPECT_TRUE((std::ranges::OutputRange<decltype(vec), int>));

    auto v1 = vec | adaptor;

    EXPECT_TRUE(std::ranges::InputRange<decltype(v1)>);
    EXPECT_TRUE(std::ranges::ForwardRange<decltype(v1)>);
    EXPECT_TRUE(std::ranges::BidirectionalRange<decltype(v1)>);
    EXPECT_TRUE(std::ranges::RandomAccessRange<decltype(v1)>);
    EXPECT_TRUE(std::ranges::View<decltype(v1)>);
    EXPECT_TRUE(std::ranges::SizedRange<decltype(v1)>);
    EXPECT_TRUE(std::ranges::CommonRange<decltype(v1)>);
    EXPECT_TRUE(const_iterable_concept<decltype(v1)>);
    EXPECT_TRUE((std::ranges::OutputRange<decltype(v1), int>));

    auto v2 = vec | view::single_pass_input | adaptor;

    EXPECT_TRUE(std::ranges::InputRange<decltype(v2)>);
    EXPECT_FALSE(std::ranges::ForwardRange<decltype(v2)>);
    EXPECT_FALSE(std::ranges::BidirectionalRange<decltype(v2)>);
    EXPECT_FALSE(std::ranges::RandomAccessRange<decltype(v2)>);
    EXPECT_TRUE(std::ranges::View<decltype(v2)>);
    EXPECT_FALSE(std::ranges::SizedRange<decltype(v2)>);
    EXPECT_FALSE(std::ranges::CommonRange<decltype(v2)>);
    EXPECT_TRUE(const_iterable_concept<decltype(v2)>); // because this is always subrange
//     EXPECT_TRUE((std::ranges::OutputRange<decltype(v2), int>)); //TODO why fail?
}

// ============================================================================
//  view_drop
// ============================================================================

TEST(view_drop, regular)
{
    do_test(view::drop, "foobar");
}

TEST(view_drop, concepts)
{
    do_concepts(view::drop(3));
}

TEST(view_drop, underlying_is_shorter)
{
    std::string vec{"foobar"};
    EXPECT_NO_THROW(( view::drop(vec, 4) )); // no parsing

    std::string v;
    EXPECT_NO_THROW(( v = vec | view::single_pass_input | view::drop(4) )); // full parsing on conversion
    EXPECT_EQ("ar", v);
}

TEST(view_drop, overloads)
{
    {   // string overload
        std::string urange{"foobar"};

        auto v = view::drop(urange, 3);

        EXPECT_TRUE((std::Same<decltype(v), std::string_view>));
        EXPECT_TRUE((std::ranges::equal(v, urange.substr(3,3))));
    }

    {   // stringview overload
        std::string_view urange{"foobar"};

        auto v = view::drop(urange, 3);

        EXPECT_TRUE((std::Same<decltype(v), std::string_view>));
        EXPECT_TRUE((std::ranges::equal(v, urange.substr(3,3))));
    }

    {   // contiguous overload
        std::vector<int> urange{1, 2, 3, 4, 5, 6};

        auto v = view::drop(urange, 3);

        EXPECT_TRUE((std::Same<decltype(v), std::span<int, std::dynamic_extent>>));
        EXPECT_TRUE((std::ranges::equal(v, std::vector{4, 5, 6})));
    }

    {   // contiguous overload
        std::array<int, 6> urange{1, 2, 3, 4, 5, 6};

        auto v = view::drop(urange, 3);

        EXPECT_TRUE((std::Same<decltype(v), std::span<int, std::dynamic_extent>>));
        EXPECT_TRUE((std::ranges::equal(v, std::vector{4, 5, 6})));
    }

    {   // generic overload (random-access container)
        std::deque<int> urange{1, 2, 3, 4, 5, 6};

        auto v = view::drop(urange, 3);

        EXPECT_TRUE((std::Same<decltype(v), std::ranges::subrange<typename std::deque<int>::iterator,
                                                                  typename std::deque<int>::iterator>>));
        EXPECT_TRUE((std::ranges::equal(v, std::vector{4, 5, 6})));
    }

    {   // generic overload (bidirectional container)
        std::list<int> urange{1, 2, 3, 4, 5, 6};

        auto v = view::drop(urange, 3);

        EXPECT_TRUE((std::Same<decltype(v), std::ranges::subrange<typename std::list<int>::iterator,
                                                                  typename std::list<int>::iterator,
                                                                  std::ranges::subrange_kind::sized>>));
        EXPECT_TRUE((std::ranges::equal(v, std::vector{4, 5, 6})));
    }

    {   // generic overload (view)
        std::array<int, 6> urange{1, 2, 3, 4, 5, 6};

        auto v = urange | std::view::filter([] (int) { return true; });
        auto v2 = view::drop(v, 3);

        EXPECT_TRUE((std::Same<decltype(v2), std::ranges::subrange<std::ranges::iterator_t<decltype(v)>,
                                                                   std::ranges::sentinel_t<decltype(v)>>>));
        EXPECT_TRUE((std::ranges::equal(v2, std::vector{4, 5, 6})));
    }
}
