cmake_minimum_required(VERSION 3.20)

project(InvoiceGenerator
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PoDoFo REQUIRED)

add_library(invoice
    src/Company.cpp
    src/Customer.cpp
    src/Invoice.cpp
    src/InvoiceItem.cpp
    src/InvoicePdf.cpp
)

target_include_directories(invoice
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(invoice
    PUBLIC
        podofo::podofo
)

add_executable(invoice-example
    examples/main.cpp
)

target_link_libraries(invoice-example
    PRIVATE
        invoice
)