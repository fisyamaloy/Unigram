set(TARGET DataAccess.Repository.Client)

set(SOURCE_FILES
    Repositories.hpp
    ClientRepositoryContainer.hpp)

add_library(${TARGET} STATIC ${SOURCE_FILES})

target_link_libraries(${TARGET} PUBLIC DataAccess.Repository.Public)
