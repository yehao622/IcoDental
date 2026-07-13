#pragma once

#include "AppConfig.hpp"

class QSettings;

namespace icodental::infrastructure::config {
    class SettingsRepository {
        public:
            AppConfig load() const;
            void save(const AppConfig& config) const;

        private:
            static constexpr auto kOrganization = "IcoDental";
            static constexpr auto kApplication = "IcoDentalDesktop";
    };
}