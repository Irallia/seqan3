seqan3_test(format_bam_test.cpp CYCLIC_DEPENDING_INCLUDES include-seqan3-io-sam_file-format_sam.hpp)
seqan3_test(format_sam_test.cpp CYCLIC_DEPENDING_INCLUDES include-seqan3-io-sam_file-format_bam.hpp)
seqan3_test(sam_file_input_test.cpp)
seqan3_test(sam_file_output_test.cpp)
seqan3_test(sam_file_record_test.cpp)
seqan3_test(sam_tag_dictionary_test.cpp)
