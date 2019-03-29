#include "window-simple-main.hpp"

using namespace Mentor;

const QString Mentor::static_name = QStringLiteral("Mentor");

QSettings settings(QStringLiteral("Mentor"), QStringLiteral("PremierOBS"));

QVariant Mentor::setting_get(const QString& key, const QVariant& defaultValue)
{
	if (settings.value(key) == QVariant())
		setting_set(key, defaultValue);
	return settings.value(key, defaultValue);
}

void Mentor::setting_set(const QString& key, const QVariant& value)
{
	settings.setValue(key, value);
}

QAction* Mentor::find_action(QList<QAction*> list, const QString& text)
{
	for (QAction *action : list) {
		if (action->objectName() == text)
			return action;
	}
	for (QAction *action : list) {
		if (action->text() == text)
			return action;
	}
	return Q_NULLPTR;
}

void Mentor::set_action_visibility(QList<QAction*> list, const QString& text, bool visible)
{
	QAction *action = find_action(list, text);
	if (action)
		action->setVisible(visible);
}
