#ifndef APP_STATE_H
#define APP_STATE_H

#include <protogen/utils/utils.h>
#include <protogen/state/protogen_head_state.h>
#include <protogen/apps/protogen_app_loader.h>

namespace protogen {

class AppState final : public IToString {
public:
	enum class Mode {
		ProtogenHead,
		App,
	};

	static std::string modeToString(Mode m);

	static Mode stringToMode(const std::string& m);

	explicit AppState(const Apps& apps);
	float frameRate() const;
	Mode mode() const;
	void setMode(Mode m);
	void setActiveApp(const std::string& app_id);
	IProtogenApp * getActiveApp() const;
	const Apps& apps() const;

	ProtogenHeadState& protogenHeadState();
	const ProtogenHeadState& protogenHeadState() const;
	
	virtual std::string toString() const override;
private:
	Mode m_mode;
	ProtogenHeadState m_protogenHeadState;

	Apps m_apps;
	std::string m_activeAppId;
};

}	// namespace

#endif