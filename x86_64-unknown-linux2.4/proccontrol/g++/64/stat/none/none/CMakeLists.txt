# CMakeLists for proccontrol/g++/64/stat/none/none
set (CMAKE_CXX_FLAGS "${MUTATOR_DEFINES}  ${CXXFLAGS} ${MUTATEE_CXXFLAGS_GNU}   -g -O0 -no-pie")
set (CMAKE_CXX_FLAGS_DEBUG "")
set (CMAKE_CXX_FLAGS_RELEASE "")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
set (CMAKE_CXX_COMPILER "${M_gnu_cxx}")
include_directories("${PROJECT_SOURCE_DIR}/testsuite/src")
include_directories("${PROJECT_SOURCE_DIR}/testsuite/src/proccontrol")
add_definitions(-DSOLO_MUTATEE)
set (CMAKE_SKIP_INSTALL_RPATH TRUE)
set (CMAKE_EXE_LINKER_FLAGS "${MUTATEE_LDFLAGS_GNU}  -static")
IF (NOT ${M_gnu_cxx} MATCHES "NOTFOUND")
CHECK_MUTATEE_COMPILER ("${M_gnu_cxx}"
	"${MUTATOR_DEFINES}  ${CXXFLAGS} ${MUTATEE_CXXFLAGS_GNU}   -g -O0 -no-pie"
	"${MUTATEE_LDFLAGS_GNU}  -static"
	"CXX"
	"gnu_cxx 64 stat"
	MUTATEEgnucxx64statCXX)

IF (MUTATEEgnucxx64statCXX MATCHES "1")
include (${PROJECT_SOURCE_DIR}/x86_64-unknown-linux2.4/srclists.cmake)
if(TARGET pcontrol)
add_executable (pc_breakpoint.stat_g++_64_none_none ${SOURCE_LIST_34})
foreach (f ${SOURCE_LIST_34})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_breakpoint.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_breakpoint.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_detach.stat_g++_64_none_none ${SOURCE_LIST_11})
foreach (f ${SOURCE_LIST_11})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_detach.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_detach.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_fork.stat_g++_64_none_none ${SOURCE_LIST_0})
foreach (f ${SOURCE_LIST_0})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_fork.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_fork.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_groups.stat_g++_64_none_none ${SOURCE_LIST_41})
foreach (f ${SOURCE_LIST_41})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_groups.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_groups.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_hw_breakpoint.stat_g++_64_none_none ${SOURCE_LIST_7})
foreach (f ${SOURCE_LIST_7})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_hw_breakpoint.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_hw_breakpoint.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_irpc.stat_g++_64_none_none ${SOURCE_LIST_72})
foreach (f ${SOURCE_LIST_72})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_irpc.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_irpc.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_launch.stat_g++_64_none_none ${SOURCE_LIST_46})
foreach (f ${SOURCE_LIST_46})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_launch.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_launch.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_singlestep.stat_g++_64_none_none ${SOURCE_LIST_31})
foreach (f ${SOURCE_LIST_31})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_singlestep.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_singlestep.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_stat.stat_g++_64_none_none ${SOURCE_LIST_9})
foreach (f ${SOURCE_LIST_9})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_stat.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_stat.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_temp_detach.stat_g++_64_none_none ${SOURCE_LIST_5})
foreach (f ${SOURCE_LIST_5})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_temp_detach.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_temp_detach.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_terminate.stat_g++_64_none_none ${SOURCE_LIST_27})
foreach (f ${SOURCE_LIST_27})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_terminate.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_terminate.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_terminate_stopped.stat_g++_64_none_none ${SOURCE_LIST_53})
foreach (f ${SOURCE_LIST_53})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_terminate_stopped.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_terminate_stopped.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_thread.stat_g++_64_none_none ${SOURCE_LIST_39})
foreach (f ${SOURCE_LIST_39})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_thread.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_thread.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_thread_cont.stat_g++_64_none_none ${SOURCE_LIST_19})
foreach (f ${SOURCE_LIST_19})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_thread_cont.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_thread_cont.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
add_executable (pc_tls.stat_g++_64_none_none ${SOURCE_LIST_60})
foreach (f ${SOURCE_LIST_60})
	get_source_file_property(lang ${f} LANGUAGE)
	if(lang MATCHES "C")
		set_source_files_properties(${f} PROPERTIES LANGUAGE CXX)
	endif()
	endforeach()
set_target_properties(pc_tls.stat_g++_64_none_none PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (pc_tls.stat_g++_64_none_none ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})


INSTALL (TARGETS
		pc_breakpoint.stat_g++_64_none_none
		pc_detach.stat_g++_64_none_none
		pc_fork.stat_g++_64_none_none
		pc_groups.stat_g++_64_none_none
		pc_hw_breakpoint.stat_g++_64_none_none
		pc_irpc.stat_g++_64_none_none
		pc_launch.stat_g++_64_none_none
		pc_singlestep.stat_g++_64_none_none
		pc_stat.stat_g++_64_none_none
		pc_temp_detach.stat_g++_64_none_none
		pc_terminate.stat_g++_64_none_none
		pc_terminate_stopped.stat_g++_64_none_none
		pc_thread.stat_g++_64_none_none
		pc_thread_cont.stat_g++_64_none_none
		pc_tls.stat_g++_64_none_none
	DESTINATION ${INSTALL_DIR})

ENDIF()
ENDIF()
ENDIF()
