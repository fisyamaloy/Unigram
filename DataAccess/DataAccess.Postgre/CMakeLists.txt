set(TARGET DataAccess.Postgre)

set(SOURCE_FILES
    PostgreAdapter.hpp
    PostgreAdapter.cpp
    PostgreRepositories.hpp
    PostgreRepositories.cpp
	PostgreRepositoryContainer.hpp
    PostgreRepositoryManager.hpp
    PostgreTable.hpp
    DataBaseOptions.hpp
    UsersAmountFinder.hpp
    UsersAmountFinder.cpp)

add_library(${TARGET} STATIC ${SOURCE_FILES})

target_link_libraries(${TARGET}
        PUBLIC CONAN_PKG::libpq CONAN_PKG::libpqxx
        PUBLIC Utility.Public DataAccess.Public Network.Public)
