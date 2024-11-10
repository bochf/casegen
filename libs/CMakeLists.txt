aux_source_directory(. lib_traveller_srcs)
add_library(traveller ${lib_traveller_srcs})
target_include_directories(traveller PUBLIC .)
