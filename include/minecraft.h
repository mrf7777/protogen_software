#ifndef MINECRAFT_H
#define MINECRAFT_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include <variant>
#include <vector>
#include <optional>
#include <utility>
#include <cmath>
#include <random>
#include <algorithm>

#include <visitor_helper.h>

namespace mc
{
    class AirBlock final
    {
    };

    class StoneBlock final
    {
    };

    class DirtBlock final
    {
    };

    class WoodBlock final
    {
    };

    class GrassBlock final
    {
    };

    class SandBlock final
    {
    };

    class WaterBlock final
    {
    };

    class Block final
    {
    public:
        using BlockVariant = std::variant<AirBlock, StoneBlock, DirtBlock, WoodBlock, GrassBlock, SandBlock, WaterBlock>;
        Block();
        explicit Block(const BlockVariant &variant);
        BlockVariant block() const;
        static Block fromString(const std::string& s);
        static std::vector<std::string> allBlockStrings();
        static std::string allBlocksSeperatedByNewline();

    private:
        BlockVariant m_block;
    };

    // A function type mapping from block to RGB color tuple.
    using BlockColorProfile = std::function<std::tuple<uint8_t, uint8_t, uint8_t>(const Block&)>;

    std::tuple<uint8_t, uint8_t, uint8_t> defaultBlockColorProfile(const Block& b);

    std::string colorHexCodeFromColor(const std::tuple<uint8_t, uint8_t, uint8_t>& color);

    class BlockMatrix
    {
    public:
        BlockMatrix(std::size_t rows, std::size_t cols);
        std::optional<Block> get(std::size_t r, std::size_t c) const;
        bool set(std::size_t r, std::size_t c, Block b);
        std::size_t rows() const;
        std::size_t cols() const;
        BlockMatrix slice(std::size_t row, std::size_t col, std::size_t rows, std::size_t cols) const;
        template <typename BlockVariant>
        bool isAll() const {
            for(const auto& block : m_blocks)
            {
                if(!std::holds_alternative<BlockVariant>(block.block()))
                {
                    return false;
                }
            }
            return true;
        }

    private:
        std::size_t rowColToIndex(std::size_t r, std::size_t c) const;
        bool rowColValid(std::size_t r, std::size_t c) const;

        std::size_t m_rows;
        std::size_t m_cols;
        std::vector<Block> m_blocks;
    };

    BlockMatrix evolveBlockMatrix(const BlockMatrix &a);

    class BlockMatrixGenerator
    {
    public:
        BlockMatrixGenerator(std::size_t rows, std::size_t cols);

        BlockMatrix generate(std::size_t seed) const;

    public:
        static void fillBlocksColumn(BlockMatrix &bm, const Block &b, std::size_t column, std::size_t row_start, std::size_t row_end);
        static void generateStructures(BlockMatrix &bm, std::size_t seed);
        static void generateDungeons(BlockMatrix &bm, std::size_t seed);
        static void tryGenerateDungeon(BlockMatrix &bm, std::size_t row, std::size_t col, std::size_t chess_relative_col);
        std::vector<std::size_t> generateRowMap(std::size_t seed) const;
        static std::vector<double> movingAverage(const std::vector<double> &vec);

        template <typename T, typename U>
        static std::vector<T> castVector(const std::vector<U> &u)
        {
            std::vector<T> t;
            for (const auto &item : u)
            {
                t.push_back(static_cast<T>(item));
            }
            return t;
        }

        std::size_t m_rows;
        std::size_t m_cols;
        double m_rowVelocityMax;
        double m_rowVelocityMin;
        std::size_t m_maxRow;
        std::size_t m_minRow;
        std::size_t m_seaLevel;
    };
}

#endif
