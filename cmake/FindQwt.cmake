find_library(QWT_LIBRARY qwt
  HINTS /usr/local/qwt/lib /usr/local/qwt-6.1.4/lib /usr/local/qwt-6.1.3/lib
  )

find_path(QWT_INCLUDE_PATH qwt.h
  HINTS /usr/local/qwt/include /usr/local/qwt-6.1.4/include /usr/local/qwt-6.1.3/include
  )
