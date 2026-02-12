cmake_minimum_required(VERSION 3.16)
project(ISR_Qt VERSION 0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets Network)

add_executable(isr_qt
    qt/src/main.cpp
    qt/src/mainwindow.cpp
    qt/src/mainwindow.h
    qt/src/portion.h
    qt/src/net_pris_client.cpp
    qt/src/net_pris_client.h
    qt/src/dialogs/simple_dialog.cpp
    qt/src/dialogs/simple_dialog.h
    qt/src/dialogs/dialogs.cpp
    qt/src/dialogs/dialogs.h
    qt/src/dialogs/checkspo_dialog.cpp
    qt/src/dialogs/checkspo_dialog.h
    qt/src/dialogs/child_window_dialog.cpp
    qt/src/dialogs/child_window_dialog.h
    qt/src/dialogs/about_dialog.cpp
    qt/src/dialogs/about_dialog.h
    qt/src/dialogs/completing_dialog.cpp
    qt/src/dialogs/completing_dialog.h
    qt/src/dialogs/close_report_dialog.cpp
    qt/src/dialogs/close_report_dialog.h
    qt/src/dialogs/partition_report_dialog.cpp
    qt/src/dialogs/partition_report_dialog.h
    qt/src/dialogs/selection_report_dialog.cpp
    qt/src/dialogs/selection_report_dialog.h
    qt/src/dialogs/partition_list_dialog.cpp
    qt/src/dialogs/partition_list_dialog.h
    qt/src/dialogs/stencil_dialog.cpp
    qt/src/dialogs/stencil_dialog.h
    qt/src/core/runtime_state.h
    qt/src/dialogs/find_dialog.cpp
    qt/src/dialogs/find_dialog.h
    qt/src/dialogs/variant_dialog.cpp
    qt/src/dialogs/variant_dialog.h
    qt/src/dialogs/request_dialog.cpp
    qt/src/dialogs/request_dialog.h
    qt/src/dialogs/title_dialog.cpp
    qt/src/dialogs/title_dialog.h
    qt/src/dialogs/settings_dialog.cpp
    qt/src/dialogs/settings_dialog.h
    qt/src/dialogs/end_dialog.cpp
    qt/src/dialogs/end_dialog.h
    qt/src/dialogs/sp_dialog.cpp
    qt/src/dialogs/sp_dialog.h
    qt/src/core/protocol_processor.cpp
    qt/src/core/protocol_processor.h
    qt/src/core/protocol_events.h
    qt/src/core/dialog_manager.cpp
    qt/src/core/dialog_manager.h
    qt/src/core/por_codec.cpp
    qt/src/core/por_codec.h
    qt/src/core/protocol_naming.cpp
    qt/src/core/protocol_naming.h
    qt/src/core/report_slots.cpp
    qt/src/core/report_slots.h
    qt/src/core/stencil_rules.cpp
    qt/src/core/stencil_rules.h
)

target_link_libraries(isr_qt PRIVATE Qt5::Widgets Qt5::Network)
