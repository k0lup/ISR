#pragma once

namespace protocol_events {

constexpr int kRodWork = 1;
constexpr int kRodTitle = 2;

namespace title {
constexpr int kSortSetTitle = 1;
constexpr int kAtomTypeTitle = 6;
} // namespace title

namespace sort {
constexpr int kMainAnswer = 2;
constexpr int kCloseApp = 3;
constexpr int kPartitionPrepared = 4;
constexpr int kMakeOperation = 5;
constexpr int kEndPartition = 6;
constexpr int kPrisLoaded = 7;
constexpr int kEdDirectory = 8;
constexpr int kNasrDirectory = 9;
constexpr int kResult = 10;
} // namespace sort

} // namespace protocol_events
