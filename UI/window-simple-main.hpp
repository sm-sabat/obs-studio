#pragma once

#include "window-basic-main.hpp"
#include <QSettings>
#include <QDebug>
#include "window-simple-about.hpp"

namespace Mentor {
	extern const QString static_name;
	extern QVariant setting_get(const QString& key, const QVariant& defaultValue = QVariant());
	extern void setting_set(const QString& key, const QVariant& value);
	extern QAction* find_action(QList<QAction*> list, const QString& text);
	extern void set_action_visibility(QList<QAction*> list, const QString& text, bool visible = false);
}

class OBSSimple : public OBSBasic
{
public:
	explicit OBSSimple(QWidget *parent = 0);
	virtual ~OBSSimple();
	void OBSInit() override;

private slots:
	void visitMentor_triggered();
	void aboutMentor_triggered();

private:
	void RefreshSceneCollections() override;
	void RefreshProfiles() override;
	QPointer<MentorAbout> aboutMentor;
};

