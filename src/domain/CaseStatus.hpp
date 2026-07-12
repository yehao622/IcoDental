#pragma once

#include <QString>
#include <optional>

namespace icodental::domain {

	enum class CaseStatus {
		New,
		Cached,
		Processing,
		Reviewed,
		Error
	};

	QString toString(CaseStatus status);
	std::optional<CaseStatus> caseStatusFromString(const QString& value);
}