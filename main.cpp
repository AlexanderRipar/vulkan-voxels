#include "voxel_volume.hpp"

#include <och_fmt.h>
#include <och_fio.h>

#include <Windows.h>

int main(int argc, const char** argv)
{
    och::utf8_string app_directory;

    och::status rst;

    if (rst = och::get_application_directory(app_directory))
        goto FAIL;

    if (rst = och::set_current_directory(app_directory.raw_cbegin()))
        goto FAIL;

    if (rst = run_voxel_volume(argc, argv))
        goto FAIL;
    
    och::print("Exited successfully\n");

    return 0;

FAIL:

    och::print("\nAn Error occurred!\n\nError {0} / 0x{0:X} of type {1}\n\n{2}\n\nCallstack:\n\n", rst.errcode(), rst.errtype_name(), rst.description());
        
    och::range<const och::error_context> callstack = och::err::get_callstack();

    for (const auto& call : callstack)
        och::print("{:40} : {:40}    {} @ {}\n", call.function(), call.line_content(), call.line_number(), call.filename());

    return 1;
}
