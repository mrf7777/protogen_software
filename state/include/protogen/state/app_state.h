#ifndef APP_STATE_H
#define APP_STATE_H

#include <string>
#include <optional>
#include <map>
#include <set>

#include <protogen/utils/utils.h>
#include <protogen/apps/ProtogenAppInitializer.h>

namespace protogen {

class AppState final : public IToString {
public:
	explicit AppState();
	bool addApp(std::shared_ptr<IProtogenApp> app);
	bool setActiveApp(const std::string& app_id);
	std::optional<std::shared_ptr<IProtogenApp>> getApp(const std::string& app_id);
	std::optional<std::shared_ptr<IProtogenApp>> getActiveApp();
	std::optional<std::string> getActiveAppId() const;
	std::set<std::string> appIds() const;
	
	virtual std::string toString() const override;
private:
	std::map<std::string, std::shared_ptr<IProtogenApp>> m_apps;
	std::optional<std::string> m_activeAppId;
};

}	// namespace

#endif