// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

#if __has_include(<seqan/seq_io.h>)
    #include <seqan/seq_io.h>
#endif

#include <algorithm>
#include <cctype>
#include <cstring>

#include <benchmark/benchmark.h>

#include <range/v3/algorithm/equal.hpp>
#include <range/v3/view/transform.hpp>

#include <seqan3/alphabet/quality/all.hpp>
#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/io/sequence_file/input_format_concept.hpp>
#include <seqan3/io/sequence_file/output.hpp>
#include <seqan3/io/sequence_file/output_format_concept.hpp>
#include <seqan3/io/sequence_file/format_fasta.hpp>
#include <seqan3/range/view/convert.hpp>
#include <seqan3/test/performance/units.hpp>

#include <sstream>

using namespace seqan3;
using namespace seqan3::test;

static constexpr size_t iterations_per_run = 4096;

static std::string fasta_line{">seq\nACTAGACTAGCTACGATCAGCTACGATCAGCTACGA\n"};

static std::string fasta_file = []()
{
    std::string file{};
    for (size_t idx = 0; idx < iterations_per_run; idx++)
        file += fasta_line;
    return file;
}();

void write3(benchmark::State & state)
{
    std::ostringstream ostream;
    sequence_file_format_fasta format;
    sequence_file_output_options options;
    std::string id{"seq"};
    dna5_vector seq{"ACTAGACTAGCTACGATCAGCTACGATCAGCTACGA"_dna5};

    for (auto _ : state)
    {
        for (size_t i = 0; i < iterations_per_run; ++i)
            format.write(ostream, options, seq, id, std::ignore);
    }

    ostream = std::ostringstream{};
    format.write(ostream, options, seq, id, std::ignore);
    size_t bytes_per_run = ostream.str().size() * iterations_per_run;
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = bytes_per_second(bytes_per_run);
}

BENCHMARK(write3);

#if __has_include(<seqan/seq_io.h>)

void write2(benchmark::State & state)
{
    std::ostringstream ostream;
    seqan::CharString id = "seq";
    seqan::Dna5String seq = "ACTAGACTAGCTACGATCAGCTACGATCAGCTACGA";

    for (auto _ : state)
    {
        for (size_t i = 0; i < iterations_per_run; ++i)
            seqan::writeRecord(ostream, id, seq, seqan::Fasta());
    }

    ostream = std::ostringstream{};
    seqan::writeRecord(ostream, id, seq, seqan::Fasta());
    size_t bytes_per_run = ostream.str().size() * iterations_per_run;
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = bytes_per_second(bytes_per_run);
}

BENCHMARK(write2);
#endif

void read3(benchmark::State & state)
{
    std::string id;
    dna5_vector seq;

    sequence_file_format_fasta format;
    sequence_file_input_options<dna5, false> options;
    for (auto _ : state)
    {
        state.PauseTiming();
        std::istringstream istream{fasta_file};
        state.ResumeTiming();

        for (size_t i = 0; i < iterations_per_run; ++i)
        {
            format.read(istream, options, seq, id, std::ignore);
            id.clear();
            seq.clear();
        }
    }

    size_t bytes_per_run = fasta_line.size() * iterations_per_run;
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = bytes_per_second(bytes_per_run);
}
BENCHMARK(read3);

#if __has_include(<seqan/seq_io.h>)

#include <fstream>

void read2(benchmark::State & state)
{
    seqan::CharString id;
    seqan::Dna5String seq;

    std::istringstream istream{};

    auto restart_iterator = [&istream]()
    {
        istream = std::istringstream{fasta_file};
        seqan::VirtualStream<char, seqan::Input> comp;
        open(comp, istream);
        return seqan::directionIterator(comp, seqan::Input());
    };

    for (auto _ : state)
    {
        state.PauseTiming();
        auto it = restart_iterator();
        state.ResumeTiming();

        for (size_t i = 0; i < iterations_per_run; ++i)
        {
            readRecord(id, seq, it, seqan::Fasta{});
            clear(id);
            clear(seq);
        }
    }

    size_t bytes_per_run = fasta_line.size() * iterations_per_run;
    state.counters["iterations_per_run"] = iterations_per_run;
    state.counters["bytes_per_run"] = bytes_per_run;
    state.counters["bytes_per_second"] = bytes_per_second(bytes_per_run);
}
BENCHMARK(read2);
#endif

BENCHMARK_MAIN();
