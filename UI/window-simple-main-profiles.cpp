#include "window-simple-main.hpp"
#include "qt-wrappers.hpp"
#include <QDir>
#include <QStandardPaths>

bool EnumProfiles(std::function<bool (const char *, const char *)> &&cb)
{
	char path[512];
	os_glob_t *glob;

	int ret = GetConfigPath(path, sizeof(path),
			"obs-studio/basic/profiles/*");
	if (ret <= 0) {
		blog(LOG_WARNING, "Failed to get profiles config path");
		return false;
	}

	if (os_glob(path, 0, &glob) != 0) {
		blog(LOG_WARNING, "Failed to glob profiles");
		return false;
	}

	bool found = false;
	for (size_t i = 0; i < glob->gl_pathc; i++) {
		const char *filePath = glob->gl_pathv[i].path;
		const char *dirName = strrchr(filePath, '/') + 1;

		if (!glob->gl_pathv[i].directory)
			continue;

		if (strcmp(dirName,  ".") == 0 ||
		    strcmp(dirName, "..") == 0)
			continue;

		std::string file = filePath;
		file += "/basic.ini";

		ConfigFile config;
		int ret = config.Open(file.c_str(), CONFIG_OPEN_EXISTING);
		if (ret != CONFIG_SUCCESS)
			continue;

		const char *name = config_get_string(config, "General", "Name");
		if (!name)
			name = strrchr(filePath, '/') + 1;

		if (QString::fromUtf8(name) != Mentor::static_name) {
			continue;
		}
		else {
			found = true;
		}

		if (!cb(name, filePath))
			break;
	}

	os_globfree(glob);

	return found;
}

void OBSSimple::RefreshProfiles()
{
	QList<QAction*> menuActions = ui->profileMenu->actions();
	int count = 0;

	for (int i = 0; i < menuActions.count(); i++) {
		QVariant v = menuActions[i]->property("file_name");
		if (v.typeName() != nullptr)
			delete menuActions[i];
	}

	const char *curName = config_get_string(App()->GlobalConfig(),
			"Basic", "Profile");

	auto addProfile = [&](const char *name, const char *path)
	{
		std::string file = strrchr(path, '/') + 1;

		QAction *action = new QAction(QT_UTF8(name), this);
		action->setProperty("file_name", QT_UTF8(path));
		connect(action, &QAction::triggered,
				this, &OBSBasic::ChangeProfile);
		action->setCheckable(true);

		action->setChecked(strcmp(name, curName) == 0);

		ui->profileMenu->addAction(action);
		count++;
		return true;
	};

	bool found = EnumProfiles(addProfile);

	ui->actionRemoveProfile->setEnabled(count > 1);

	if (!found) {
		QDir qdir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
		bool path_ok = qdir.cdUp();
		if (path_ok)
			path_ok = qdir.cd(Mentor::static_name);
		if (path_ok) {
			char path[512];

			int ret = GetConfigPath(path, 512, "obs-studio/basic/profiles/");
			if (ret > 0) {
				QString dir = qdir.absolutePath();
				if (!dir.isEmpty() && !dir.isNull()) {
					QString inputPath = QString::fromUtf8(path);
					QFileInfo finfo(dir);
					QString directory = finfo.fileName();
					QString profileDir = inputPath + directory;
					QDir folder(profileDir);
					if (!folder.exists()) {
						folder.mkpath(profileDir);
						QFile::copy(dir + "/basic.ini",
							profileDir + "/basic.ini");
						QFile::copy(dir + "/service.json",
							profileDir + "/service.json");
						QFile::copy(dir + "/streamEncoder.json",
							profileDir + "/streamEncoder.json");
						QFile::copy(dir + "/recordEncoder.json",
							profileDir + "/recordEncoder.json");
						RefreshProfiles();
						return;
					}
				}
			}
			else {
				blog(LOG_WARNING, "Failed to get profile config path");
			}
		}
	}

	QAction *mentorAction = Mentor::find_action(ui->profileMenu->actions(), Mentor::static_name);
	if (mentorAction)
		mentorAction->trigger();
}
