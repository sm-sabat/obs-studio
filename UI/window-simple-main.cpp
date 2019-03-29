#include "window-simple-main.hpp"

#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QLocale>

#include "obs-app.hpp"

using namespace Mentor;

void OBSSimple::visitMentor_triggered()
{
	QString website;
	if (QLocale::system().language() == QLocale::Polish || QLocale::system().country() ==QLocale::Poland) {
		website = QStringLiteral("https://www.mentorpolska.pl");
	}
	else {
		website = QStringLiteral("https://www.mentorpolska.pl/en");
	}
	QUrl url = QUrl(website, QUrl::TolerantMode);
	QDesktopServices::openUrl(url);
}

void OBSSimple::aboutMentor_triggered()
{
	if (aboutMentor)
		aboutMentor->close();

	aboutMentor = new MentorAbout(this);
	aboutMentor->show();

	aboutMentor->setAttribute(Qt::WA_DeleteOnClose, true);
}

void OBSSimple::OBSInit()
{
	OBSBasic::OBSInit();
	multiviewProjectorMenu->menuAction()->setVisible(false);
	Mentor::set_action_visibility(findChild<QMenu*>("viewMenu")->actions(), QTStr("MultiviewWindowed"));

	QList<QAction*> alllist = findChild<QMenu*>("menuTools")->actions();
	QList<QAction*> whitelist = {
		Mentor::find_action(alllist, QStringLiteral("OutputTimer")),
		findChild<QAction*>(QStringLiteral("autoConfigure"))
	};
	for (QAction *action : alllist) {
		if (!whitelist.contains(action))
			action->setVisible(false);
	}

	findChild<QAction*>(QStringLiteral("resetUI"))->toggle();

	findChild<QAction*>(QStringLiteral("toggleScenes"))->setChecked(false);
	findChild<QAction*>(QStringLiteral("toggleTransitions"))->setChecked(false);
	findChild<QAction*>(QStringLiteral("toggleStats"))->setChecked(false);

	findChild<QAction*>(QStringLiteral("toggleMixer"))->setChecked(true);
	findChild<QAction*>(QStringLiteral("toggleControls"))->setChecked(true);
	findChild<QAction*>(QStringLiteral("toggleSources"))->setChecked(true);

	findChild<QAction*>(QStringLiteral("lockUI"))->setChecked(true);
}

OBSSimple::OBSSimple(QWidget *parent)
	: OBSBasic(parent)
{
	findChild<QAction*>(QStringLiteral("actionRemux"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionShowSettingsFolder"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionShowProfileFolder"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionCopySource"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionPasteRef"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionPasteDup"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionCopyFilters"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionPasteFilters"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionHelpPortal"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionWebsite"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionDiscord"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionUploadCurrentLog"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionUploadLastLog"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionUploadLastCrashLog"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionNewProfile"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionDupProfile"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionRenameProfile"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionRemoveProfile"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionImportProfile"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionExportProfile"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionNewSceneCollection"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionDupSceneCollection"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionRenameSceneCollection"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionRemoveSceneCollection"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionImportSceneCollection"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionExportSceneCollection"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("toggleStats"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionCheckForUpdates"))->setVisible(false);
	findChild<QAction*>(QStringLiteral("actionShowAbout"))->setVisible(false);

	findChild<QMenu*>(QStringLiteral("transformMenu"))->menuAction()->setVisible(false);
	findChild<QMenu*>(QStringLiteral("orderMenu"))->menuAction()->setVisible(false);
	findChild<QMenu*>(QStringLiteral("viewMenuDocks"))->menuAction()->setVisible(false);
	findChild<QMenu*>(QStringLiteral("viewMenuToolbars"))->menuAction()->setVisible(false);

	findChild<QPushButton*>(QStringLiteral("modeSwitch"))->setVisible(false);
	findChild<QPushButton*>(QStringLiteral("settingsButton"))->setVisible(false);
	findChild<QPushButton*>(QStringLiteral("exitButton"))->setVisible(false);

	findChild<OBSBasicPreview*>(QStringLiteral("preview"))->setContextMenuPolicy(Qt::DefaultContextMenu);
	findChild<QLabel*>(QStringLiteral("previewDisabledLabel"))->setContextMenuPolicy(Qt::DefaultContextMenu);

	QMenu *help_menu = findChild<QMenu*>(QStringLiteral("menuBasic_MainMenu_Help"));
	QAction *visitMentor = new QAction(tr("Visit Website"), this);
	connect(visitMentor, &QAction::triggered, this, &OBSSimple::visitMentor_triggered);
	help_menu->insertAction(help_menu->actions().at(0), visitMentor);
	QAction *aboutMentor = new QAction(tr("About"), this);
	connect(aboutMentor, &QAction::triggered, this, &OBSSimple::aboutMentor_triggered);
	help_menu->addAction(aboutMentor);
}

OBSSimple::~OBSSimple()
{

}
