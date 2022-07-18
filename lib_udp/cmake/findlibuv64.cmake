# 指定头文件
set (uv_compiler_name "vc14")
set (uv_platform  "x64")

set (uv_pre_full_name ${CMAKE_CURRENT_SOURCE_DIR}/ext/lib_uv/${uv_compiler_name}/${uv_platform})


# message("=========${uv_pre_full_name}")


set(tmp_inc "${uv_pre_full_name}/include")
# message("999999 =============${tmp_inc}")


# set(uv_inc_files "")
FIND_PATH(uv_inc_files uv.h ${tmp_inc})#${uv_pre_full_name}/include)



FIND_LIBRARY(uv_lib_files uv64D.lib ${uv_pre_full_name}/lib/debug)


set(uv_bin_debug_file ${uv_pre_full_name}/bin/debug/uv64D.dll)
set(uv_bin_release_file ${uv_pre_full_name}/bin/release/uv64.dll)
set(uv_inc_path 
${uv_pre_full_name}/include
)


set(lib_uv_has_found FALSE)
if (uv_inc_files AND uv_lib_files)
  set(lib_uv_has_found TRUE)
endif()
