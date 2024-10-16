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

// from: https://en.cppreference.com/w/cpp/utility/variant/visit
// helper type for the visitor #4
template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

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
        Block() : m_block(AirBlock()) {}
        explicit Block(const BlockVariant &variant) : m_block(variant) {}
        BlockVariant block() const
        {
            return m_block;
        }

        static Block fromString(const std::string& s) {
            if(s == "air")
                return Block(AirBlock());
            else if(s == "stone")
                return Block(StoneBlock());
            else if(s == "dirt")
                return Block(DirtBlock());
            else if(s == "wood")
                return Block(DirtBlock());
            else if(s == "grass")
                return Block(GrassBlock());
            else if(s == "sand")
                return Block(SandBlock());
            else if(s == "water")
                return Block(WaterBlock());
            else
                return Block(AirBlock());
        }

	static std::vector<std::string> allBlockStrings() {
		return {"air", "stone", "dirt", "wood", "grass", "sand", "water"};
	}

	static std::string allBlocksSeperatedByNewline() {
		std::string blocks;
		for(const auto& block : allBlockStrings()) {
			blocks += block;
			blocks += "\n";
		}
		return blocks;
	}

    private:
        BlockVariant m_block;
    };

    // A function type mapping from block to RGB color tuple.
    using BlockColorProfile = std::function<std::tuple<uint8_t, uint8_t, uint8_t>(const Block&)>;

    std::tuple<uint8_t, uint8_t, uint8_t> defaultBlockColorProfile(const Block& b) {
        return std::visit(overloaded{
			[](const mc::AirBlock){ return std::tuple{0, 0, 0}; },
			[](const mc::StoneBlock){ return std::tuple{127, 127, 127}; },
			[](const mc::DirtBlock){ return std::tuple{166, 81, 25}; },
			[](const mc::WoodBlock){ return std::tuple{255, 169, 41}; },
			[](const mc::GrassBlock){ return std::tuple{62, 191, 48}; },
			[](const mc::SandBlock){ return std::tuple{245, 255, 105}; },
			[](const mc::WaterBlock){ return std::tuple{87, 163, 222}; },
		}, b.block());
    }

    std::string colorHexCodeFromColor(const std::tuple<uint8_t, uint8_t, uint8_t>& color) {
        std::stringstream ss;
        ss << "#" << std::hex << std::setfill('0')
        << std::setw(2) << static_cast<int>(std::get<0>(color))
        << std::setw(2) << static_cast<int>(std::get<1>(color))
        << std::setw(2) << static_cast<int>(std::get<2>(color));
        return ss.str();
    }

    class BlockMatrix
    {
    public:
        BlockMatrix(std::size_t rows, std::size_t cols) : m_rows(rows), m_cols(cols), m_blocks(rows * cols) {}
        std::optional<Block> get(std::size_t r, std::size_t c) const
        {
            if (!rowColValid(r, c))
            {
                return {};
            }

            return m_blocks.at(rowColToIndex(r, c));
        }
        bool set(std::size_t r, std::size_t c, Block b)
        {
            if (!rowColValid(r, c))
            {
                return false;
            }

            m_blocks.at(rowColToIndex(r, c)) = b;
            return true;
        }
        std::size_t rows() const
        {
            return m_rows;
        }
        std::size_t cols() const
        {
            return m_cols;
        }
        BlockMatrix slice(std::size_t row, std::size_t col, std::size_t rows, std::size_t cols) const
        {
            // cut rows and cols if too large
            if(row + rows > this->rows())
            {
                rows = this->rows() - row;
            }
            if(col + cols > this->cols())
            {
                cols = this->cols() - col;
            }
            BlockMatrix slice(rows, cols);
            const std::size_t end_row = row + rows;
            const std::size_t end_col = col + cols;
            for(std::size_t r = row; r < end_row; r++)
            {
                for(std::size_t c = col; c < end_col; c++)
                {
                    slice.set(r - row, c - col, m_blocks.at(rowColToIndex(r, c)));
                }
            }
            return slice;
        }
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
        std::size_t rowColToIndex(std::size_t r, std::size_t c) const
        {
            return m_cols * r + c;
        }
        bool rowColValid(std::size_t r, std::size_t c) const
        {
            return r < m_rows && c < m_cols;
        }

        std::size_t m_rows;
        std::size_t m_cols;
        std::vector<Block> m_blocks;
    };

    BlockMatrix evolveBlockMatrix(const BlockMatrix &a)
    {
        BlockMatrix b(a.rows(), a.cols());

        for (std::size_t r = 0; r < a.rows(); r++)
        {
            for (std::size_t c = 0; c < a.cols(); c++)
            {
                const Block block = a.get(r, c).value();
                const Block new_block = std::visit(overloaded{[=](const AirBlock &air)
                                                              {
                                                                  // sand fall population rule
                                                                  // If there is sand above in original matrix, populate sand here.
                                                                  const auto block_above_original = a.get(r - 1, c);
                                                                  if (block_above_original.has_value())
                                                                  {
                                                                      if (std::holds_alternative<SandBlock>(block_above_original.value().block()))
                                                                      {
                                                                          return Block(SandBlock());
                                                                      }
                                                                  }

                                                                  return Block(air);
                                                              },
                                                              [](const StoneBlock &stone)
                                                              { return Block(stone); },
                                                              [](const DirtBlock &dirt)
                                                              { return Block(dirt); },
                                                              [](const WoodBlock &wood)
                                                              { return Block(WoodBlock()); },
                                                              [](const GrassBlock &grass)
                                                              { return Block(grass); },
                                                              [=](const SandBlock &sand)
                                                              {
                                                                  // sand fall removal rule
                                                                  // If there is air below in original matrix, remove sand here.
                                                                  const auto block_below_original = a.get(r + 1, c);
                                                                  if (block_below_original.has_value())
                                                                  {
                                                                      if (std::holds_alternative<AirBlock>(block_below_original.value().block()))
                                                                      {
                                                                          return Block(AirBlock());
                                                                      }
                                                                  }
                                                                  return Block(sand);
                                                              },
                                                              [](const WaterBlock &water)
                                                              { return Block(water); }},
                                                   block.block());
                b.set(r, c, new_block);
            }
        }

        return b;
    }

    class BlockMatrixGenerator
    {
    public:
        BlockMatrixGenerator(std::size_t rows, std::size_t cols)
            : m_rows(rows),
              m_cols(cols),
              m_rowVelocityMax(2.0),
              m_rowVelocityMin(-2.0),
              m_maxRow(rows - 2),
              m_minRow(2),
              m_seaLevel(rows - 8)
        {
        }

        BlockMatrix generate(std::size_t seed) const
        {
            const auto row_map = generateRowMap(seed);

            BlockMatrix block_matrix(m_rows, m_cols);
            // some rules:
            // - If column is exposed to air, use grass block.
            // - If column is exposed to water, use sand block.
            for (std::size_t c = 0; c < row_map.size(); c++)
            {
                const auto row = row_map.at(c);
                if (row >= m_seaLevel)
                {
                    fillBlocksColumn(block_matrix, Block(WaterBlock()), c, m_seaLevel, m_rows);
                    fillBlocksColumn(block_matrix, Block(SandBlock()), c, row, row + 3);
                    fillBlocksColumn(block_matrix, Block(StoneBlock()), c, row + 3, m_rows);
                }
                else
                {
                    fillBlocksColumn(block_matrix, Block(GrassBlock()), c, row, row + 1);
                    fillBlocksColumn(block_matrix, Block(DirtBlock()), c, row + 1, row + 3);
                    fillBlocksColumn(block_matrix, Block(StoneBlock()), c, row + 3, m_rows);
                }
            }

            generateStructures(block_matrix, seed);

            return block_matrix;
        }

    public:
        static void fillBlocksColumn(BlockMatrix &bm, const Block &b, std::size_t column, std::size_t row_start, std::size_t row_end)
        {
            if (row_end < row_start)
            {
                std::swap(row_start, row_end);
            }

            for (std::size_t r = row_start; r < row_end; r++)
            {
                bm.set(r, column, b);
            }
        }
        static void generateStructures(BlockMatrix &bm, std::size_t seed)
        {
            generateDungeons(bm, seed);
        }
        static void generateDungeons(BlockMatrix &bm, std::size_t seed)
        {
            std::mt19937 rng(seed);
            std::bernoulli_distribution dist(0.001);   // how often to try for dungeon
            std::uniform_int_distribution chess_dist(0, 2); // where to place chess/wood. 0: left, 1: middle, 2: right
            for(std::size_t r = 0; r < bm.rows(); r++)
            {
                for(std::size_t c = 0; c < bm.cols(); c++)
                {
                    if(dist(rng))
                    {
                        const std::size_t chess_relative_column = chess_dist(rng);
                        tryGenerateDungeon(bm, r, c, chess_relative_column);
                    }
                }
            }
        }
        static void tryGenerateDungeon(BlockMatrix &bm, std::size_t row, std::size_t col, std::size_t chess_relative_col)
        {
            // Rectangle dungeon.
            constexpr std::size_t ROWS = 3;
            constexpr std::size_t COLS = 3;
            // Test it can fit and can be placed in stone.
            if(bm.slice(row, col, ROWS, COLS).isAll<StoneBlock>())
            {
                // Generate Dungeon
                const std::size_t end_row = row + ROWS;
                const std::size_t end_col = col + COLS;
                for(std::size_t r = row; r < end_row; r++)
                {
                    for(std::size_t c = col; c < end_col; c++)
                    {
                        bm.set(r, c, Block(AirBlock()));
                    }
                }

                // place chess
                bm.set(row + ROWS - 1, col + chess_relative_col, Block(WoodBlock()));
            }
        }
        std::vector<std::size_t> generateRowMap(std::size_t seed) const
        {
            // choose starting row
            const std::size_t center = m_rows / 2;

            // generate a row level for each column
            std::vector<double> float_rows;
            std::mt19937 rng(seed);
            std::normal_distribution accel_dist{0.0, 1.0};
            double vel = 0.0;
            double acceleration = 0.0;
            double current = center;
            for (std::size_t c = 0; c < m_cols; c++)
            {
                float_rows.push_back(current);

                // calculate the next row level
                acceleration = accel_dist(rng);
                vel += acceleration;
                vel = std::clamp(vel, m_rowVelocityMin, m_rowVelocityMax);
                if (vel == m_rowVelocityMin || vel == m_rowVelocityMax)
                {
                    vel = 0;
                }
                current += vel;
                current = std::clamp(current, static_cast<double>(m_minRow), static_cast<double>(m_maxRow));
            }

            const std::vector<std::size_t> rows = castVector<std::size_t, double>(movingAverage(movingAverage(float_rows)));

            return rows;
        }

        static std::vector<double> movingAverage(const std::vector<double> &vec)
        {
            if (vec.empty() || vec.size() == 1)
            {
                return vec;
            }

            std::vector<double> r;
            for (std::size_t i = 0; i < vec.size(); i++)
            {
                if (i == 0)
                {
                    r.push_back(0.5 * (vec.at(0) + vec.at(1)));
                }
                else if (i == vec.size() - 1)
                {
                    r.push_back(0.5 * (vec.at(i - 1) + vec.at(i)));
                }
                else
                {
                    r.push_back((vec.at(i - 1) + vec.at(i) + vec.at(i + 1)) / 3);
                }
            }

            return r;
        }

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
