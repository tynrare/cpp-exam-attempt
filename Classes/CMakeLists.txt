add_library (Classes
  Index.h
  FirstTest.cpp
	SecondTest.cpp
  ThirdTest.cpp
	FourthTest.cpp
	)

install (TARGETS Classes DESTINATION bin)
install (FILES Index.h DESTINATION include)
