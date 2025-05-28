
add_subdirectory(analysis)
add_subdirectory(filter)
add_subdirectory(matching)
add_subdirectory(stacking)
add_subdirectory(testimages)

if (FFTW_FOUND)
add_subdirectory(fft)
add_subdirectory(lucyrichardson)
add_subdirectory(vancittert)
endif()
