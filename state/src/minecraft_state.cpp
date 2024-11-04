#include <protogen/state/minecraft_state.h>

namespace protogen {

MinecraftPlayerState::CursorDirection MinecraftPlayerState::stringToCursorDirection(const std::string& cd) {
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

MinecraftPlayerState::MinecraftPlayerState(std::size_t start_row, std::size_t start_col, std::size_t max_row, std::size_t max_col)
    : m_cursor{start_row, start_col},
    m_maxRow{max_row},
    m_maxCol{max_col},
    m_selectedBlock(Block(DirtBlock()))
{
    correctCursor();
}

Block MinecraftPlayerState::selectedBlock() const {
    return m_selectedBlock;
}

void MinecraftPlayerState::setSelectedBlock(const Block& b) {
    m_selectedBlock = b;
}

MinecraftPlayerState::CursorPos MinecraftPlayerState::cursor() const {
    return m_cursor;
}

void MinecraftPlayerState::moveCursor(CursorDirection direction) {
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

void MinecraftPlayerState::correctCursor() {
    // If cursor is out of bounds, put it back in bounds.
    if(m_cursor.first >= m_maxRow)
        m_cursor.first = m_maxRow - 1;
    if(m_cursor.second >= m_maxCol)
        m_cursor.second = m_maxCol - 1;
}

MinecraftState::MinecraftState()
    : m_blockMatrix(32, 128),
    m_blockColorProfile(defaultBlockColorProfile),
    m_players()
{}

const BlockMatrix& MinecraftState::blockMatrix() const { return m_blockMatrix; }

BlockMatrix& MinecraftState::blockMatrix() { return m_blockMatrix; }

const BlockColorProfile MinecraftState::blockColorProfile() const { return m_blockColorProfile; }

bool MinecraftState::addNewPlayer(const PlayerId& id) {
    std::lock_guard<std::mutex> lock(m_playerMutex);
    if(m_players.contains(id)) {
        return false;
    } else {
        const std::size_t number_players = m_players.size();
        // TODO: result of insert is not used. Should we check this 
        // and return boolean? Is'nt this going away soon?
        m_players.insert(std::make_pair(
            id,
            MinecraftPlayerState(0, number_players, 32, 128)
        ));
        return true;
    }
}

void MinecraftState::removePlayer(const PlayerId& id) {
    std::lock_guard<std::mutex> lock(m_playerMutex);
    m_players.erase(id);
}

bool MinecraftState::accessPlayer(const PlayerId& id, std::function<void(MinecraftPlayerState&)> accessor) {
    std::lock_guard<std::mutex> lock(m_playerMutex);
    if(m_players.contains(id)) {
        accessor(m_players.at(id));
        return true;
    } else {
        return false;
    }
}

bool MinecraftState::accessPlayer(const PlayerId& id, std::function<void(const MinecraftPlayerState&)> accessor) const {
    std::lock_guard<std::mutex> lock(m_playerMutex);
    if(m_players.contains(id)) {
        accessor(m_players.at(id));
        return true;
    } else {
        return false;
    }
}

std::vector<MinecraftState::PlayerId> MinecraftState::players() const {
    std::lock_guard<std::mutex> lock(m_playerMutex);
    return _players();
}

std::string MinecraftState::playersSeparatedByNewline() const {
    std::lock_guard<std::mutex> lock(m_playerMutex);
    std::string players_string;
    for(const auto& player : _players()) {
        players_string += player + "\n";
    }
    return players_string;
}

std::vector<MinecraftState::PlayerId> MinecraftState::_players() const {
    std::vector<PlayerId> players;
    for(const auto& player : m_players) {
        players.push_back(player.first);
    }
    return players;
}

}   // namespace