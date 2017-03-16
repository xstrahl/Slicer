
#
# This custom version of 'msvc9compiler.py' has been made by:
#
# (1) applying the following patches to the version of 'msvc9compiler.py' shipped
#     with Python-2.7.10.tgz:
#
#       0001-distutils-msvc9compiler-Add-support-for-Visual-C-Com.patch
#
#
# (2) copying the 'msvc.py' file copied from pypa/setuptools@907fc81 (34.3.2) and
#     applying the following patches:
#
#       0001-setuptools-msvc-Update-to-allow-reuse-from-python2.7.patch
#

#
# msvc9compiler.py
#
set(in_msvc9compiler ${CMAKE_CURRENT_LIST_DIR}/python_patched_msvc9compiler.py)
set(out_msvc9compiler ${PYTHON_SRC_DIR}/Lib/distutils/msvc9compiler.py)

message("Copying patched 'Lib/distutils/msvc9compiler.py' into source directory [${PYTHON_SRC_DIR}]
  in_msvc9compiler:${in_msvc9compiler}
  out_msvc9compiler:${out_msvc9compiler}"
  )

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${in_msvc9compiler} ${out_msvc9compiler})

#
# msvc.py
#
set(in_msvc ${CMAKE_CURRENT_LIST_DIR}/python_patched_msvc.py)
set(out_msvc ${PYTHON_SRC_DIR}/Lib/distutils/msvc.py)

message("Copying patched 'Lib/distutils/msvc9compiler.py' into source directory [${PYTHON_SRC_DIR}]
  in_msvc:${in_msvc}
  out_msvc:${out_msvc}"
  )

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${in_msvc} ${out_msvc})
