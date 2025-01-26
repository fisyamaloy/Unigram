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

find_package(libpqxx)
find_package(PostgreSQL)

target_link_libraries(${TARGET}
        PUBLIC PostgreSQL::PostgreSQL libpqxx::pqxx
        PUBLIC Utility.Public DataAccess.Public Network.Public)
