//
// AI Generated tests for Steganographer Parameters
//

#include <gtest/gtest.h>

#include <filesystem>
#include <string>
#include <vector>

#include "utils/Parameters.h"

namespace fs = std::filesystem;

namespace
{
    struct Argv
    {
        std::vector<std::string> storage;
        std::vector<char*> ptrs;

        explicit Argv(std::initializer_list<std::string> args)
            : storage(args)
        {
            ptrs.reserve(storage.size());
            for (auto& s : storage)
                ptrs.push_back(s.data());
        }

        int ac() const { return static_cast<int>(ptrs.size()); }
        char** av() { return ptrs.data(); }
    };
}

TEST(ParametersCliTest, ParseEncodeString)
{
    const fs::path img = fs::path(TEST_DATA_DIR) / "img.png";
    const fs::path out = fs::path(TEST_DATA_DIR) / "out_encode_string.png";

    Argv a{
        "stegnocode",
        "encode",
        "string",
        "Hello, World!",
        img.string(),
        out.string()
    };

    Parameters p(a.ac(), a.av());

    EXPECT_EQ(p.get_mode(), ENCODE);
    EXPECT_EQ(p.get_type(), DataType::STRING);
    ASSERT_EQ(p.get_parameters().size(), 1u);
    EXPECT_EQ(p.get_parameters()[0], "Hello, World!");
    EXPECT_EQ(p.get_image_path(), img.string());
    EXPECT_EQ(p.get_output_path(), out.string());
}

TEST(ParametersCliTest, ParseEncodeFile)
{
    const fs::path img = fs::path(TEST_DATA_DIR) / "img.png";
    const fs::path out = fs::path(TEST_DATA_DIR) / "out_encode_file.png";
    const fs::path payload = fs::path(TEST_DATA_DIR) / "payload.txt";

    Argv a{
        "stegnocode",
        "encode",
        "file",
        payload.string(),
        img.string(),
        out.string()
    };

    Parameters p(a.ac(), a.av());

    EXPECT_EQ(p.get_mode(), ENCODE);
    EXPECT_EQ(p.get_type(), DataType::FILE_T);
    ASSERT_EQ(p.get_parameters().size(), 1u);
    EXPECT_EQ(p.get_parameters()[0], payload.string());
    EXPECT_EQ(p.get_image_path(), img.string());
    EXPECT_EQ(p.get_output_path(), out.string());
}

TEST(ParametersCliTest, ParseDecode)
{
    const fs::path encoded = fs::path(TEST_DATA_DIR) / "encoded.png";

    Argv a{
        "stegnocode",
        "decode",
        encoded.string()
    };

    Parameters p(a.ac(), a.av());

    EXPECT_EQ(p.get_mode(), DECODE);
    EXPECT_EQ(p.get_image_path(), encoded.string());
}

TEST(ParametersCliTest, MissingArgsEncodeThrows)
{
    Argv a{
        "stegnocode",
        "encode",
        "string"
        // manque payload + input + output
    };

    EXPECT_THROW(
        {
            Parameters p(a.ac(), a.av());
            (void)p;
        },
        ParametersError
    );
}