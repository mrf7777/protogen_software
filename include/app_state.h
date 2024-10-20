#ifndef APP_STATE_H
#define APP_STATE_H

#include <utils.h>
#include <minecraft_state.h>
#include <protogen_head_state.h>

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

#endif