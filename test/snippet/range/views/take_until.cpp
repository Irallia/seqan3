#include <seqan3/std/ranges>                            // for std::views::reverse
#include <string>

#include <seqan3/core/debug_stream.hpp>                 // for debug_stream
#include <seqan3/range/views/single_pass_input.hpp>     // for views::single_pass_input
#include <seqan3/range/views/take_until.hpp>            // for views::take_until*
#include <seqan3/utility/char_operations/predicate.hpp> // for is_char

int main()
{
    // regular usage
    std::string vec{"foo\nbar"};
    auto v = vec | seqan3::views::take_until(seqan3::is_char<'\n'>); // or use a lambda
    seqan3::debug_stream << v << '\n'; // "foo"

    auto v2 = vec | std::views::reverse | seqan3::views::take_until(seqan3::is_char<'\n'>);
    seqan3::debug_stream << v2 << '\n'; // "rab"

    // consuming behaviour
    std::string vec2{"foo      bar"}; // ← multiple spaces
    auto vin = vec2 | seqan3::views::single_pass_input;
    auto v3 = vin | seqan3::views::take_until_and_consume(seqan3::is_blank);
    seqan3::debug_stream << v3 << '\n'; // "foo"
    seqan3::debug_stream << *std::ranges::begin(vin) << '\n'; // "b", the spaces where skipped
}
