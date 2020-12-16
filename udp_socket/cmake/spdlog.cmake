
#-------------------------------------------------------------------
include(fetchcontent)
 fetchcontent_declare(	spdlog	#库名字
						GIT_REPOSITORY https://gitee.com/mohistH/spdlog.git	# 仓库地址
						GIT_TAG v1.x # 库版本
						SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/spdlog # 指定库下载地址
						)

fetchcontent_makeavailable(spdlog)
