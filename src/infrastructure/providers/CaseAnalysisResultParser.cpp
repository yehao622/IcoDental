#include "infrastructure/providers/CaseAnalysisResultParser.hpp"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

namespace {
    QString normalizeStructuredJson(QString text) {
        text = text.trimmed();

        if (!text.startsWith("```")) {
            return text;
        }

        const int firstNewline = text.indexOf('\n');
        if (firstNewline < 0) {
            return text;
        }

        text = text.mid(firstNewline + 1);

        const int closingFence = text.lastIndexOf("```");
        if (closingFence >= 0) {
            text = text.left(closingFence);
        }

        return text.trimmed();
    }
}

namespace icodental::infrastructure::providers {
    using icodental::domain::CaseAnalysisResult;
    
    ProviderResponse CaseAnalysisResultParser::parse(const QByteArray& responseBytes) const {
        const QString rawResponse = QString::fromUtf8(responseBytes);

        const QJsonDocument envelopeDocument = QJsonDocument::fromJson(responseBytes);
        if (!envelopeDocument.isObject()) {
            return ProviderResponse(false, QString(), rawResponse, "Gemini response was not a JSON object.");
        }

        const QJsonObject root = envelopeDocument.object();
        const QJsonArray candidates = root.value("candidates").toArray();
        if (candidates.isEmpty()) {
            return ProviderResponse(false, QString(), rawResponse, "Gemini response contained no candidates.");
        }

        const QJsonObject contentObject = candidates.first().toObject().value("content").toObject();
        const QJsonArray responseParts = contentObject.value("parts").toArray();
        if (responseParts.isEmpty()) {
            return ProviderResponse(false, QString(), rawResponse, "Gemini response contained no text parts.");
        }

        const QString structuredText = responseParts.first().toObject().value("text").toString().trimmed();
        if (structuredText.isEmpty()) {
            return ProviderResponse(false, QString(), rawResponse, "Gemini response text was empty.");
        }

        return parseStructuredText(structuredText, rawResponse);
    }

    ProviderResponse CaseAnalysisResultParser::parseStructuredText(
        const QString& structuredText,
        const QString& rawResponse) const
    {
        const QString normalizedText = normalizeStructuredJson(structuredText);
        const QJsonDocument resultDocument = QJsonDocument::fromJson(normalizedText.toUtf8());

        if (!resultDocument.isObject()) {
            return ProviderResponse(
                false,
                QString(),
                rawResponse,
                "Analysis result text was not valid JSON.");
        }

        const CaseAnalysisResult result = parseResultObject(resultDocument.object(), normalizedText);

        if (!result.isValid()) {
            return ProviderResponse(
                false,
                QString(),
                rawResponse,
                "Case analysis result was invalid.");
        }

        return ProviderResponse(
            true,
            result.rawProviderText(),
            rawResponse,
            QString(),
            result);
    }

    CaseAnalysisResult CaseAnalysisResultParser::parseResultObject(
        const QJsonObject& jsonObject,
        const QString& structuredText) const
    {
        const QString rawProviderText = jsonObject.value("rawProviderText").toString().trimmed();

        return CaseAnalysisResult(
            jsonObject.value("doctorName").toString(),
            jsonObject.value("officeName").toString(),
            jsonObject.value("patientName").toString(),
            jsonObject.value("caseType").toString(),
            jsonObject.value("toothNumber").toString(),
            jsonObject.value("shade").toString(),
            jsonObject.value("specialInstructions").toString(),
            jsonObject.value("confidenceNote").toString(),
            rawProviderText.isEmpty() ? structuredText : rawProviderText);
    }
}