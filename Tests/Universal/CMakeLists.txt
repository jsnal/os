add_executable(TestArray ../../Universal/Stdlib.cpp TestArray.cpp)
add_executable(TestArrayList ../../Universal/Stdlib.cpp TestArrayList.cpp)
add_executable(TestByteBuffer ../../Universal/Stdlib.cpp TestByteBuffer.cpp)
add_executable(TestCircularQueue ../../Universal/Stdlib.cpp TestCircularQueue.cpp)
add_executable(TestNumber ../../Universal/Stdlib.cpp TestNumber.cpp)
add_executable(TestOptional ../../Universal/Stdlib.cpp TestOptional.cpp)
add_executable(TestSharedPtr ../../Universal/Stdlib.cpp TestSharedPtr.cpp)
add_executable(TestString
    TestString.cpp
    ../../Universal/Stdlib.cpp
    ../../Universal/String.cpp
    ../../Universal/PrintFormat.cpp
)
add_executable(TestStringView ../../Universal/Stdlib.cpp TestStringView.cpp)
add_executable(TestBasicString
    TestBasicString.cpp
    ../../Universal/Stdlib.cpp
    ../../Universal/BasicString.cpp
)
