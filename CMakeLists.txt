cmake_minimum_required(VERSION 3.21)
project(VotingORM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/utf-8 /bigobj /Zc:__cplusplus)
endif()

find_package(SqliteOrm CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(VotingORM
  src/main.cpp
  src/ComparisonDemo.cpp
  src/RelationshipDemo.cpp
  src/RepositoryDemo.cpp
  src/QueryDemos.cpp
  src/TransactionDemos.cpp
  src/RawSqlite.cpp
  src/SQLiteCandidateRepository.cpp
  src/VoteDao.cpp
  src/TestData.cpp
)

target_include_directories(VotingORM
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(VotingORM
  PRIVATE
    sqlite_orm::sqlite_orm
    unofficial::sqlite3::sqlite3
)
