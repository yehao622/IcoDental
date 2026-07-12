#include "CaseStatus.hpp"

namespace icodental::domain {
	QString toString(CaseStatus status) {
		switch(status) {
			case CaseStatus::New: return "new";
			case CaseStatus::Cached: return "cached";
			case CaseStatus::Processing: return "processing";
			case CaseStatus::Reviewed: return "reviewed";
			case CaseStatus::Error: return "error";
		}
		return "error";
	}

	std::optional<CaseStatus> caseStatusFromString(const QString& value) {
		if (value == "new") return CaseStatus::New;
		if (value == "cached") return CaseStatus::Cached;
	    if (value == "processing") return CaseStatus::Processing;
	    if (value == "reviewed") return CaseStatus::Reviewed;
	    if (value == "error") return CaseStatus::Error;
	    return std::nullopt;
	}
}