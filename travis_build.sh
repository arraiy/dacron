#!/bin/bash
clang_format_clean=0
for cpp_file in $(find ./ | grep -E '\.(cc|h)$')
do
    clang-format -style=file -output-replacements-xml $cpp_file | grep "<replacement " >/dev/null
    if [ $? -ne 1 ]
    then
	echo "Error, clang-format not clean: $cpp_file"
	clang_format_clean=1
    fi
done

if [ $clang_format_clean -ne 0 ]
then
    exit 1
fi


set -ex
for build_type in \
    RelWithDebInfo \
    ASan \
    TSan \
    UBSan
do
    (
	mkdir -p build-$build_type && cd build-$build_type
	cmake \
	    -DCMAKE_PREFIX_PATH=/opt/opencv_ai \
	    -DCMAKE_CXX_COMPILER=clang++ \
	    -DCMAKE_C_COMPILER=clang \
	    -DCMAKE_BUILD_TYPE=$build_type \
	    ..
	make
	ctest
    )
done
