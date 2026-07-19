#pragma once

#include "infrastructure/providers/ProviderRequest.hpp"
#include "infrastructure/providers/ProviderResponse.hpp"

namespace icodental::infrastructure::providers {
    class ProviderClient {
        public:
            virtual ~ProviderClient() = default;
            [[nodiscard]] virtual ProviderResponse analyze(const ProviderRequest& request) = 0;
    };
}