# 用法（由 CMakeLists 调用）：
#   cmake -DSRC_DIR=... -DOUT_DIR=... -DSTAMP_FILE=... -P sync_shaders.cmake
#
# 目标：
# - 计算 SRC_DIR 下所有文件的内容 hash（递归）
# - 与 STAMP_FILE 里记录的旧 hash 比较
# - 只有内容变化时才 copy_directory 到 OUT_DIR，并更新 STAMP_FILE

if(NOT DEFINED SRC_DIR OR NOT DEFINED OUT_DIR OR NOT DEFINED STAMP_FILE)
  message(FATAL_ERROR "sync_shaders.cmake requires -DSRC_DIR -DOUT_DIR -DSTAMP_FILE")
endif()

if(NOT EXISTS "${SRC_DIR}")
  # 源目录不存在就直接退出（不算错误）
  message(STATUS "sync_shaders: SRC_DIR not found, skip: ${SRC_DIR}")
  return()
endif()

# 递归收集文件（忽略目录）
file(GLOB_RECURSE _files LIST_DIRECTORIES false "${SRC_DIR}/*")

# 计算组合 hash：对每个文件做 SHA256，再拼接后做一次 SHA256
set(_concat "")
foreach(_f IN LISTS _files)
  if(EXISTS "${_f}")
    file(SHA256 "${_f}" _h)
    string(APPEND _concat "${_f}:${_h}\n")
  endif()
endforeach()
string(SHA256 _newHash "${_concat}")

set(_oldHash "")
if(EXISTS "${STAMP_FILE}")
  file(READ "${STAMP_FILE}" _oldHash)
  string(STRIP "${_oldHash}" _oldHash)
endif()

if(_newHash STREQUAL _oldHash)
  message(STATUS "sync_shaders: unchanged (hash=${_newHash})")
  return()
endif()

# 有变化才同步
file(MAKE_DIRECTORY "${OUT_DIR}")
execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_directory "${SRC_DIR}" "${OUT_DIR}")

# 写入新 hash
file(WRITE "${STAMP_FILE}" "${_newHash}\n")
message(STATUS "sync_shaders: updated -> ${OUT_DIR} (hash=${_newHash})")


