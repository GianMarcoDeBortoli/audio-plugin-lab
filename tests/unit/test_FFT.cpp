#include <gtest/gtest.h>

#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "dsp/FFT.h"

using apl::dsp::FFT;

// ================================================================
// TYPE TRAITS

TEST(FFTTest, IsNotCopyConstructible)
{
    EXPECT_FALSE(std::is_copy_constructible_v<FFT>);
}

TEST(FFTTest, IsNotCopyAssignable)
{
    EXPECT_FALSE(std::is_copy_assignable_v<FFT>);
}

TEST(FFTTest, IsMoveConstructible)
{
    EXPECT_TRUE(std::is_move_constructible_v<FFT>);
}

TEST(FFTTest, IsNothrowMoveConstructible)
{
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<FFT>);
}

TEST(FFTTest, IsNothrowMoveAssignable)
{
    EXPECT_TRUE(std::is_nothrow_move_assignable_v<FFT>);
}


// ================================================================
// CONSTRUCTOR

TEST(FFTTest, ConstructorRejectsZeroOrder)
{
    EXPECT_THROW(
        FFT fft(0),
        std::invalid_argument
    );
}

TEST(FFTTest, ConstructorCreatesCorrectSize)
{
    FFT fft(4);

    EXPECT_EQ(
        fft.getNfft(),
        std::size_t { 16 }
    );
}

TEST(FFTTest, ConstructorCreatesCorrectNumberOfBins)
{
    FFT fft(4);

    EXPECT_EQ(
        fft.getNumBins(),
        std::size_t { 9 }
    );
}

TEST(FFTTest, ConstructorCreatesCorrectBufferSize)
{
    FFT fft(4);

    EXPECT_EQ(
        fft.getBufferSize(),
        std::size_t { 32 }
    );
}


// ================================================================
// BIN ACCESS

TEST(FFTTest, SetBinValueRejectsInvalidDataSize)
{
    FFT fft(4);

    const std::size_t bufferSize = fft.getBufferSize();

    std::vector<float> data(bufferSize, 0.0f);

    EXPECT_THROW(
        fft.setBinValue(
            data.data(),
            bufferSize + 1,
            0,
            3.5f,
            0.0f
        ),
        std::invalid_argument
    );

    EXPECT_THROW(
        fft.setBinValue(
            data.data(),
            bufferSize - 1,
            0,
            3.5f,
            0.0f
        ),
        std::invalid_argument
    );

    EXPECT_THROW(
        fft.setBinValue(
            data.data(),
            bufferSize * 2,
            0,
            3.5f,
            0.0f
        ),
        std::invalid_argument
    );

    EXPECT_THROW(
        fft.setBinValue(
            data.data(),
            bufferSize / 2,
            0,
            3.5f,
            0.0f
        ),
        std::invalid_argument
    );
}

TEST(FFTTest, SetBinValueRejectsInvalidDCValue)
{
    FFT fft(4);

    const std::size_t bufferSize = fft.getBufferSize();

    std::vector<float> data(bufferSize, 0.0f);

    EXPECT_THROW(
        fft.setBinValue(
            data.data(),
            bufferSize,
            0,
            3.5f,
            1.0f
        ),
        std::invalid_argument
    );
}

TEST(FFTTest, SetAndGetDCBin)
{
    FFT fft(4);

    const std::size_t bufferSize = fft.getBufferSize();

    std::vector<float> data(bufferSize, 0.0f);

    fft.setBinValue(
        data.data(),
        bufferSize,
        0,
        3.5f,
        0.0f
    );

    EXPECT_FLOAT_EQ(
        fft.getBinReal(
                data.data(),
                bufferSize,
                0
            ),
        3.5f
    );

    EXPECT_FLOAT_EQ(
        fft.getBinImag(
                data.data(),
                bufferSize,
                0
            ),
        0.0f
    );
}

TEST(FFTTest, SetBinValueRejectsInvalidNYValue)
{
    FFT fft(4);

    const std::size_t Nfft = fft.getNfft();
    const std::size_t bufferSize = fft.getBufferSize();

    std::vector<float> data(bufferSize, 0.0f);

    EXPECT_THROW(
        fft.setBinValue(
            data.data(),
            bufferSize,
            Nfft/2,
            3.5f,
            1.0f
        ),
        std::invalid_argument
    );
}

TEST(FFTTest, SetAndGetNYBin)
{
    FFT fft(4);

    const std::size_t Nfft = fft.getNfft();
    const std::size_t bufferSize = fft.getBufferSize();

    std::vector<float> data(bufferSize, 0.0f);

    fft.setBinValue(
        data.data(),
        bufferSize,
        Nfft/2,
        2.5f,
        0.0f
    );

    EXPECT_FLOAT_EQ(
        fft.getBinReal(
            data.data(),
            bufferSize,
            Nfft/2
        ),
        2.5f
    );

    EXPECT_FLOAT_EQ(
        fft.getBinImag(
            data.data(),
            bufferSize,
            Nfft/2
        ),
        0.0f
    );
}

TEST(FFTTest, SetAndGetInteriorBin)
{
    FFT fft(4);

    const std::size_t bufferSize = fft.getBufferSize();

    std::vector<float> data(bufferSize, 0.0f);

    constexpr std::size_t k = 3;

    fft.setBinValue(
        data.data(),
        bufferSize,
        k,
        1.25f,
        -2.5f
    );

    EXPECT_FLOAT_EQ(
        fft.getBinReal(
            data.data(),
            bufferSize,
            k
        ),
        1.25f
    );

    EXPECT_FLOAT_EQ(
        fft.getBinImag(
            data.data(),
            bufferSize,
            k
        ),
        -2.5f
    );
}

TEST(FFTTest, SetBinRejectsOutOfRangeBin)
{
    FFT fft(4);

    const std::size_t bufferSize = fft.getBufferSize();

    std::vector<float> data(bufferSize, 0.0f);

    EXPECT_THROW(
        fft.setBinValue(
            data.data(),
            bufferSize,
            fft.getNumBins(),
            1.0f,
            0.0f
        ),
        std::out_of_range
    );
}

TEST(FFTTest, GetRealRejectsOutOfRangeBin)
{
    FFT fft(4);

    const std::size_t bufferSize = fft.getBufferSize();

    std::vector<float> data(bufferSize, 0.0f);

    EXPECT_THROW(
        fft.getBinReal(
            data.data(),
            bufferSize,
            fft.getNumBins()
        ),
        std::out_of_range
    );
}

TEST(FFTTest, GetImagRejectsOutOfRangeBin)
{
    FFT fft(4);

    const std::size_t bufferSize = fft.getBufferSize();

    std::vector<float> data(bufferSize, 0.0f);

    EXPECT_THROW(
        fft.getBinImag(
            data.data(),
            bufferSize,
            fft.getNumBins()
        ),
        std::out_of_range
    );
}


// ================================================================
// FORWARD / INVERSE TRANSFORM

TEST(FFTTest, RFFTOfConstantSignalHasOnlyDC)
{
    FFT fft(3);

    const std::size_t Nfft = fft.getNfft();
    const std::size_t bufferSize = fft.getBufferSize();

    std::vector<float> data(bufferSize, 0.0f);

    for (std::size_t n = 0; n < Nfft; ++n)
        data[n] = 1.0f;

    fft.rfft(data.data(), bufferSize);

    EXPECT_NEAR(
        fft.getBinReal(data.data(), bufferSize, 0),
        static_cast<float>(Nfft),
        1e-5f
    );

    EXPECT_NEAR(
        fft.getBinImag(data.data(), bufferSize, 0),
        0.0f,
        1e-5f
    );

    for (std::size_t k = 1; k < fft.getNumBins(); ++k)
    {
        EXPECT_NEAR(
            fft.getBinReal(data.data(), bufferSize, k),
            0.0f,
            1e-5f
        );

        EXPECT_NEAR(
            fft.getBinImag(data.data(), bufferSize, k),
            0.0f,
            1e-5f
        );
    }
}

TEST(FFTTest, RFFTOfImpulseHasConstantSpectrum)
{
    FFT fft(3);

    const std::size_t Nfft = fft.getNfft();
    const std::size_t bufferSize = fft.getBufferSize();

    std::vector<float> data(bufferSize, 0.0f);

    data[0] = 1.0f;

    fft.rfft(data.data(), data.size());

    for (std::size_t k = 0; k < fft.getNumBins(); ++k)
    {
        EXPECT_NEAR(
            fft.getBinReal(data.data(), bufferSize, k),
            1.0f,
            1e-5f
        );

        EXPECT_NEAR(
            fft.getBinImag(data.data(), bufferSize, k),
            0.0f,
            1e-5f
        );
    }
}

TEST(FFTTest, RFFTIRFFTReconstructsSignal)
{
    FFT fft(3);

    const std::size_t Nfft = fft.getNfft();
    const std::size_t bufferSize = fft.getBufferSize();

    const std::vector<float> input {
        0.5f, -1.0f, 2.0f, 0.25f,
        -0.75f, 1.5f, -2.5f, 0.0f,
        3.0f, -0.5f, 0.75f, -1.25f,
        0.25f, 2.0f, -1.5f, 0.5f
    };

    ASSERT_EQ(input.size(), bufferSize);

    std::vector<float> data(bufferSize, 0.0f);

    for (std::size_t n = 0; n < Nfft; ++n)
        data[n] = input[n];

    for (std::size_t i = 0; i < bufferSize; ++i)
        std::cout << "data[" << i << "] = " << data[i] << '\n';

    fft.rfft(data.data(), data.size());
    fft.irfft(data.data(), data.size());

    for (std::size_t i = 0; i < bufferSize; ++i)
        std::cout << "data[" << i << "] = " << data[i] << '\n';

    for (std::size_t n = 0; n < Nfft; ++n)
    {
        EXPECT_NEAR(
            data[n],
            input[n],
            1e-5f
        );
    }
}


// ================================================================
// BIN ACCESS AFTER FFT

TEST(FFTTest, BinAccessMatchesKnownImpulseSpectrum)
{
    FFT fft(4);

    const std::size_t Nfft = fft.getNfft();
    const std::size_t bufferSize = fft.getBufferSize();

    std::vector<float> data(bufferSize, 0.0f);

    data[0] = 1.0f;

    fft.rfft(data.data(), bufferSize);

    for (std::size_t k = 0; k < fft.getNumBins(); ++k)
    {
        EXPECT_NEAR(
            fft.getBinReal(data.data(), bufferSize, k),
            1.0f,
            1e-5f
        );

        EXPECT_NEAR(
            fft.getBinImag(data.data(), bufferSize, k),
            0.0f,
            1e-5f
        );
    }
}
