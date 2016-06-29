#include "gtest/gtest.h"

#include "scraps/AverageRate.h"

#include <chrono>

using namespace scraps;

TEST(TimeValueSamples, basicUsage) {
    TimeValueSamples<uintmax_t> timeValues{3};
    EXPECT_EQ(timeValues.size(), 0);

    timeValues.insert(std::chrono::steady_clock::time_point(1s), 1000); EXPECT_EQ(timeValues.size(), 1);
    timeValues.insert(std::chrono::steady_clock::time_point(2s), 2000); EXPECT_EQ(timeValues.size(), 2);
    timeValues.insert(std::chrono::steady_clock::time_point(3s), 3000); EXPECT_EQ(timeValues.size(), 3);
    timeValues.insert(std::chrono::steady_clock::time_point(4s), 4000); EXPECT_EQ(timeValues.size(), 3); // older sample was erased
    timeValues.insert(std::chrono::steady_clock::time_point(5s), 5000); EXPECT_EQ(timeValues.size(), 3); // older sample was erased

    EXPECT_EQ(timeValues.samples(), (std::map<std::chrono::steady_clock::time_point, uintmax_t>{
        {std::chrono::steady_clock::time_point(3s), 3000},
        {std::chrono::steady_clock::time_point(4s), 4000},
        {std::chrono::steady_clock::time_point(5s), 5000},
    }));
}

TEST(TimeValueSamples, erasesOlderSamples) {
    TimeValueSamples<uintmax_t> timeValues{3};
    EXPECT_EQ(timeValues.size(), 0);

    timeValues.insert(std::chrono::steady_clock::time_point(4s), 4000); EXPECT_EQ(timeValues.size(), 1);
    timeValues.insert(std::chrono::steady_clock::time_point(5s), 5000); EXPECT_EQ(timeValues.size(), 2);
    timeValues.insert(std::chrono::steady_clock::time_point(1s), 1000); EXPECT_EQ(timeValues.size(), 3);
    timeValues.insert(std::chrono::steady_clock::time_point(2s), 2000); EXPECT_EQ(timeValues.size(), 3); // 1s erased
    timeValues.insert(std::chrono::steady_clock::time_point(3s), 3000); EXPECT_EQ(timeValues.size(), 3); // 2s erased

    EXPECT_EQ(timeValues.samples(), (std::map<std::chrono::steady_clock::time_point, uintmax_t>{
        {std::chrono::steady_clock::time_point(3s), 3000},
        {std::chrono::steady_clock::time_point(4s), 4000},
        {std::chrono::steady_clock::time_point(5s), 5000},
    }));
}

TEST(AverageRate, calculatesAverages) {
    std::map<std::chrono::steady_clock::time_point, uintmax_t> samples{
        {std::chrono::steady_clock::time_point(0s),  0},
        {std::chrono::steady_clock::time_point(3s),  1000},
        {std::chrono::steady_clock::time_point(9s),  2000},
        {std::chrono::steady_clock::time_point(10s), 6000},
    };

    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples.begin(), samples.end()), 900);
    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples), 900);
    EXPECT_NEAR(*AverageRate<std::chrono::seconds>(std::next(samples.begin()), samples.end()), 1142, 5);
}

TEST(AverageRate, handlesSampleSizesOfOne) {
    std::map<std::chrono::steady_clock::time_point, uintmax_t> samples{};

    ASSERT_EQ(AverageRate<std::chrono::seconds>(samples), stdts::nullopt);
    samples.emplace(std::chrono::steady_clock::time_point(1s), 1000);
    ASSERT_EQ(AverageRate<std::chrono::seconds>(samples), stdts::nullopt); // not enough samples yet
    samples.emplace(std::chrono::steady_clock::time_point(2s), 1000);
    ASSERT_EQ(*AverageRate<std::chrono::seconds>(samples), 1000);
}

TEST(AverageRate, castsDurations) {
    std::map<std::chrono::steady_clock::time_point, uintmax_t> samples{
        {std::chrono::steady_clock::time_point(0s),  0},
        {std::chrono::steady_clock::time_point(1s),  1000},
    };

    EXPECT_EQ(*AverageRate<std::chrono::milliseconds>(samples), 1);
}

TEST(AverageRate, handlesFloatingPointResolution) {
    std::map<std::chrono::steady_clock::time_point, uintmax_t> samples{
        {std::chrono::steady_clock::time_point(0s),  0},
        {std::chrono::steady_clock::time_point(1min),  1},
    };

    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples), 1.0/60.0);
}

TEST(AverageRate, providesFloatingPointRates) {
    std::map<std::chrono::steady_clock::time_point, uintmax_t> samples{
        {std::chrono::steady_clock::time_point(0s),  0},
        {std::chrono::steady_clock::time_point(30s),  1},
        {std::chrono::steady_clock::time_point(1min),  1},
    };

    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples), 2.0/60.0);
}

TEST(AverageRate, specificIntervals) {
    std::map<std::chrono::steady_clock::time_point, uintmax_t> samples{
        {std::chrono::steady_clock::time_point(0s), 0},
        {std::chrono::steady_clock::time_point(1s), 1000},
        {std::chrono::steady_clock::time_point(2s), 2000},
    };

    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples), 1500);
    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples.begin(), samples.end()), 1500);
    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(0s), std::chrono::steady_clock::time_point(2s)), 1500);

    EXPECT_EQ(*AverageRate<std::chrono::seconds>(std::next(samples.begin()), samples.end()), 2000);
    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(1s), std::chrono::steady_clock::time_point(2s)), 2000);

    samples.emplace(std::chrono::steady_clock::time_point(3s), 4000);

    EXPECT_NEAR(*AverageRate<std::chrono::seconds>(samples), 2333, 5);
    EXPECT_NEAR(*AverageRate<std::chrono::seconds>(samples.begin(), samples.end()), 2333, 5);
    EXPECT_NEAR(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(0s), std::chrono::steady_clock::time_point(3s)), 2333, 5);

    EXPECT_EQ(*AverageRate<std::chrono::seconds>(std::next(samples.begin()), samples.end()), 3000);
    EXPECT_NEAR(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(1s), std::chrono::steady_clock::time_point(3s)), 3000, 5);
    EXPECT_NEAR(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(2s), std::chrono::steady_clock::time_point(3s)), 4000, 5);
}

TEST(AverageRate, durationCoveredBySingleSample) {
    std::map<std::chrono::steady_clock::time_point, uintmax_t> samples{
        {std::chrono::steady_clock::time_point(0s), 0},
        {std::chrono::steady_clock::time_point(1s), 1000},
        {std::chrono::steady_clock::time_point(2s), 2000},
        {std::chrono::steady_clock::time_point(3s), 3000},
        {std::chrono::steady_clock::time_point(4s), 4000},
        {std::chrono::steady_clock::time_point(5s), 5000},
        {std::chrono::steady_clock::time_point(6s), 6000},
        {std::chrono::steady_clock::time_point(7s), 7000},
        {std::chrono::steady_clock::time_point(8s), 8000},
        {std::chrono::steady_clock::time_point(9s), 9000},
        {std::chrono::steady_clock::time_point(10s), 10000},
    };

    EXPECT_EQ(*AverageRate<std::chrono::seconds>(std::next(samples.begin(), 4), std::next(samples.begin(), 6)), 5000);
    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(4s), std::chrono::steady_clock::time_point(5s)), 5000);

    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples.begin(), std::next(samples.begin(), 2)), 1000);
    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(0s), std::chrono::steady_clock::time_point(1s)), 1000);

    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(4500ms), std::chrono::steady_clock::time_point(5s)), 5000);

    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(9500ms)), 10000);

    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(9300ms), std::chrono::steady_clock::time_point(9800ms)), 10000);
}

TEST(AverageRate, durationCoversIncompleteInterval) {
    std::map<std::chrono::steady_clock::time_point, uintmax_t> samples{
        {std::chrono::steady_clock::time_point(0s), 0},
        {std::chrono::steady_clock::time_point(1s), 1000},
        {std::chrono::steady_clock::time_point(2s), 2000},
        {std::chrono::steady_clock::time_point(3s), 3000},
        {std::chrono::steady_clock::time_point(4s), 4000},
        {std::chrono::steady_clock::time_point(5s), 5000},
        {std::chrono::steady_clock::time_point(6s), 6000},
        {std::chrono::steady_clock::time_point(7s), 7000},
        {std::chrono::steady_clock::time_point(8s), 8000},
        {std::chrono::steady_clock::time_point(9s), 9000},
        {std::chrono::steady_clock::time_point(10s), 10000},
    };

    EXPECT_EQ(AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(-1s), std::chrono::steady_clock::time_point(0s)), stdts::nullopt);
    EXPECT_EQ(AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(11s), std::chrono::steady_clock::time_point(12s)), stdts::nullopt);
}

TEST(AverageRate, durationCoversExactTimePoints) {
    std::map<std::chrono::steady_clock::time_point, uintmax_t> samples{
        {std::chrono::steady_clock::time_point(0s), 0},
        {std::chrono::steady_clock::time_point(1s), 1000},
        {std::chrono::steady_clock::time_point(2s), 2000},
        {std::chrono::steady_clock::time_point(3s), 3000},
        {std::chrono::steady_clock::time_point(4s), 4000},
        {std::chrono::steady_clock::time_point(5s), 5000},
        {std::chrono::steady_clock::time_point(6s), 6000},
        {std::chrono::steady_clock::time_point(7s), 7000},
        {std::chrono::steady_clock::time_point(8s), 8000},
        {std::chrono::steady_clock::time_point(9s), 9000},
        {std::chrono::steady_clock::time_point(10s), 10000},
    };

    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(-5s), std::chrono::steady_clock::time_point(5s)), 1500);
    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(3s), std::chrono::steady_clock::time_point(5s)), 4500);
    EXPECT_EQ(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(5s)), 8000);
    EXPECT_NEAR(*AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(9s), std::chrono::steady_clock::time_point(20s)), 909, 5);
}

TEST(AverageRate, handlesZeroDurationCalculations) {
    std::map<std::chrono::steady_clock::time_point, uintmax_t> samples{
        {std::chrono::steady_clock::time_point(0s), 0},
        {std::chrono::steady_clock::time_point(1s), 1000},
    };

    EXPECT_EQ(AverageRate<std::chrono::seconds>(samples.begin(), samples.begin()), stdts::nullopt);
    EXPECT_EQ(AverageRate<std::chrono::seconds>(std::next(samples.begin()), std::next(samples.begin())), stdts::nullopt);
    EXPECT_EQ(AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(0s), std::chrono::steady_clock::time_point(0s)), stdts::nullopt);
    EXPECT_EQ(AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(1s), std::chrono::steady_clock::time_point(1s)), stdts::nullopt);
}

TEST(AverageRate, handlesSingleSamples) {
    std::map<std::chrono::steady_clock::time_point, uintmax_t> samples{
        {std::chrono::steady_clock::time_point(0s), 0},
    };

    EXPECT_EQ(AverageRate<std::chrono::seconds>(samples), stdts::nullopt);
    EXPECT_EQ(AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(0s), std::chrono::steady_clock::time_point(1s)), stdts::nullopt);
    EXPECT_EQ(AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(0s), std::chrono::steady_clock::time_point(11s)), stdts::nullopt);
    EXPECT_EQ(AverageRate<std::chrono::seconds>(samples, std::chrono::steady_clock::time_point(0s)), stdts::nullopt);
}
