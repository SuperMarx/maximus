project(maximus)

find_package(Boost COMPONENTS system program_options regex locale date_time filesystem REQUIRED)

include_directories(SYSTEM
                    ${Boost_INCLUDE_DIRS}
                    ${SuperMarx_INCLUDE_DIRS})
                    
add_library(maximuscore scraper.cpp)
target_link_libraries(maximuscore
					  supermarx-scraper
                      ${Boost_LIBRARIES}) 

add_executable(maximus main.cpp)
target_link_libraries(maximus
					  maximuscore
					  supermarx-api)

include_directories(${maximus_INCLUDE_DIRS})
