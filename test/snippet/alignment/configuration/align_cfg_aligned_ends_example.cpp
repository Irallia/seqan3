#include <seqan3/alignment/configuration/align_config_aligned_ends.hpp>
#include <seqan3/io/stream/debug_stream.hpp>

int main()
{
{
//! [access]
    using namespace seqan3;

    // Create an end_gaps object with one user defined static value and one user defined non-static value.
    align_cfg::end_gaps eg{align_cfg::first_seq_leading{std::true_type{}},
                           align_cfg::second_seq_leading{true}};

    // Check if the first_seq_leading parameter contains static information.
    if constexpr (decltype(eg)::is_static<0>())
    {
        debug_stream << "The leading gaps of the first sequence are static and the value is: " <<
                        std::boolalpha << decltype(eg)::get_static<0>() << '\n';
    }

    // Defaulted parameters will always be false and static.
    debug_stream << "The leading gaps of the first sequence are static and the value is " <<
                    std::boolalpha << decltype(eg)::get_static<0>() << '\n';

    // Non-static parameters won't be captured as static. Trying to access it via get_static will trigger a static assert.
    if constexpr (!decltype(eg)::is_static<2>())
    {
        debug_stream << "The leading gaps of the second sequence is not static! The value is: " <<
                        std::boolalpha << eg[2] << '\n';
    }

    debug_stream << "The value can always be determined at runtime like for the trailing gaps of the second sequence: "
                 << std::boolalpha << eg[3] << '\n';
//! [access]
}
{
//! [aligned_ends]
    using namespace seqan3;

    // Setup for overlap alignment.
    align_cfg::aligned_ends overlap{align_cfg::all_ends_free};

    // Setup for global alignment.
    align_cfg::aligned_ends global{align_cfg::none_ends_free};

    // Setup for semi-global alignment with free-end gaps in the first sequence.
    align_cfg::aligned_ends semi_seq1{align_cfg::seq1_ends_free};

    // Setup for semi-global alignment with free-end gaps in the second sequence.
    align_cfg::aligned_ends semi_seq2{align_cfg::seq2_ends_free};

    // Custom settings.
    align_cfg::aligned_ends custom{align_cfg::end_gaps{align_cfg::first_seq_leading{std::true_type{}},
                                                       align_cfg::second_seq_leading{std::true_type{}}}};
//! [aligned_ends]

    (void) overlap;
    (void) global;
    (void) semi_seq1;
    (void) semi_seq2;
    (void) custom;
}
}
