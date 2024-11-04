#ifndef APP_STATE_H
#define APP_STATE_H

#include <protogen/utils/utils.h>
#include <protogen/state/minecraft_state.h>
#include <protogen/state/protogen_head_state.h>

namespace protogen {

class AppState final : public IToString {
public:
	enum class Mode {
		ProtogenHead,
		Minecraft,
	};

	static std::string modeToString(Mode m);

	static Mode stringToMode(const std::string& m);
	static std::vector<Mode> allModes();
	AppState();
	int frameRate() const;
	Mode mode() const;
	void setMode(Mode m);
	ProtogenHeadState& protogenHeadState();
	const ProtogenHeadState& protogenHeadState() const;
	MinecraftState& minecraftState();
	const MinecraftState& minecraftState() const;
	
	virtual std::string toString() const override;
private:
	Mode m_mode;
	ProtogenHeadState m_protogenHeadState;
	MinecraftState m_minecraftState;
};

}	// namespace

#endif