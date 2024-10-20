#ifndef MINECRAFT_STATE_H
#define MINECRAFT_STATE_H

#include <utility>
#include <string>
#include <mutex>
#include <map>

#include <minecraft.h>

class MinecraftPlayerState final {
public:
	enum class CursorDirection {
		Up, Down, Left, Right,
	};
	static CursorDirection stringToCursorDirection(const std::string& cd) {
		if(cd == "up")
			return CursorDirection::Up;
		else if(cd == "down")
			return CursorDirection::Down;
		else if(cd == "left")
			return CursorDirection::Left;
		else if(cd == "right")
			return CursorDirection::Right;
		else
			return CursorDirection::Up;
	}
	using CursorPos = std::pair<std::size_t, std::size_t>;
	MinecraftPlayerState(std::size_t start_row, std::size_t start_col, std::size_t max_row, std::size_t max_col)
		: m_cursor{start_row, start_col},
		m_maxRow{max_row},
		m_maxCol{max_col},
		m_selectedBlock(mc::Block(mc::DirtBlock()))
	{
		correctCursor();
	}
	mc::Block selectedBlock() const {
		return m_selectedBlock;
	}
	void setSelectedBlock(const mc::Block& b) {
		m_selectedBlock = b;
	}
	CursorPos cursor() const {
		return m_cursor;
	}
	void moveCursor(CursorDirection direction) {
		switch(direction) {
		case CursorDirection::Up:
			if(m_cursor.first > 0)
				m_cursor.first -= 1;
			break;
		case CursorDirection::Down:
			if(m_cursor.first < (m_maxRow - 1))
				m_cursor.first += 1;
			break;
		case CursorDirection::Left:
			if(m_cursor.second > 0)
				m_cursor.second -= 1;
			break;
		case CursorDirection::Right:
			if(m_cursor.second < (m_maxCol - 1))
				m_cursor.second += 1;
			break;
		}
	}
private:
	void correctCursor() {
		// If cursor is out of bounds, put it back in bounds.
		if(m_cursor.first >= m_maxRow)
			m_cursor.first = m_maxRow - 1;
		if(m_cursor.second >= m_maxCol)
			m_cursor.second = m_maxCol - 1;
	}

	CursorPos m_cursor;
	std::size_t m_maxRow;
	std::size_t m_maxCol;
	mc::Block m_selectedBlock;
};

class MinecraftState final {
public:
	using PlayerId = std::string;

	MinecraftState()
		: m_blockMatrix(32, 128), 
		m_players(),
		m_blockColorProfile(mc::defaultBlockColorProfile)
	{}
	const mc::BlockMatrix& blockMatrix() const { return m_blockMatrix; }
	mc::BlockMatrix& blockMatrix() { return m_blockMatrix; }
	const mc::BlockColorProfile blockColorProfile() const { return m_blockColorProfile; }
	bool addNewPlayer(const PlayerId& id) {
		std::lock_guard<std::mutex> lock(m_playerMutex);
		if(m_players.contains(id)) {
			return false;
		} else {
			const std::size_t number_players = m_players.size();
			const auto result = m_players.insert(std::make_pair(
				id,
				MinecraftPlayerState(0, number_players, 32, 128)
			));
			return true;
		}
	}
	void removePlayer(const PlayerId& id) {
		std::lock_guard<std::mutex> lock(m_playerMutex);
		m_players.erase(id);
	}
	bool accessPlayer(const PlayerId& id, std::function<void(MinecraftPlayerState&)> accessor) {
		std::lock_guard<std::mutex> lock(m_playerMutex);
		if(m_players.contains(id)) {
			accessor(m_players.at(id));
			return true;
		} else {
			return false;
		}
	}
	bool accessPlayer(const PlayerId& id, std::function<void(const MinecraftPlayerState&)> accessor) const {
		std::lock_guard<std::mutex> lock(m_playerMutex);
		if(m_players.contains(id)) {
			accessor(m_players.at(id));
			return true;
		} else {
			return false;
		}
	}
	std::vector<PlayerId> players() const {
		std::lock_guard<std::mutex> lock(m_playerMutex);
		return _players();
	}
	std::string playersSeperatedByNewline() const {
		std::lock_guard<std::mutex> lock(m_playerMutex);
		std::string players_string;
		for(const auto player : _players()) {
			players_string += player + "\n";
		}
		return players_string;
	}
private:
	std::vector<PlayerId> _players() const {
		std::vector<PlayerId> players;
		for(const auto& player : m_players) {
			players.push_back(player.first);
		}
		return players;
	}

	mc::BlockMatrix m_blockMatrix;
	mc::BlockColorProfile m_blockColorProfile;
	mutable std::mutex m_playerMutex;
	std::map<PlayerId, MinecraftPlayerState> m_players;
};

#endif