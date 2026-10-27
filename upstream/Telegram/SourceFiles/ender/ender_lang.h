// This is the source code of EnderGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#pragma once

#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>

class EnderLanguage : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(EnderLanguage)

public:
	static EnderLanguage *currentInstance();
	static void init();
	static EnderLanguage *instance;

	void fetchLanguage(const QString &id, const QString &baseId);
	void applyLanguageJson(QJsonDocument doc);

public Q_SLOTS:
	void fetchFinished();
	void fetchError(QNetworkReply::NetworkError e);

private:
	EnderLanguage();
	~EnderLanguage() override = default;

	void loadCachedLanguage();
	void saveCachedLanguage(const QByteArray &json, const QString &langId);
	[[nodiscard]] QString getCacheDir() const;
	[[nodiscard]] QString getCachePath(const QString &langId) const;

	QNetworkAccessManager networkManager;
	QNetworkReply *_chkReply = nullptr;
	bool needFallback = false;
	QString _currentLangId;
};
