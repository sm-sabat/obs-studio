#include "window-simple-main.hpp"
#include "qt-wrappers.hpp"
#include <QDir>
#include <QStandardPaths>
#include <QImage>

using namespace std;

bool EnumSceneCollections(std::function<bool (const char *, const char *)> &&cb)
{
	char path[512];
	os_glob_t *glob;

	int ret = GetConfigPath(path, sizeof(path),
			"obs-studio/basic/scenes/*.json");
	if (ret <= 0) {
		blog(LOG_WARNING, "Failed to get config path for scene "
		                  "collections");
		return false;
	}

	if (os_glob(path, 0, &glob) != 0) {
		blog(LOG_WARNING, "Failed to glob scene collections");
		return false;
	}

	bool found = false;
	for (size_t i = 0; i < glob->gl_pathc; i++) {
		const char *filePath = glob->gl_pathv[i].path;

		if (glob->gl_pathv[i].directory)
			continue;

		obs_data_t *data = obs_data_create_from_json_file_safe(filePath,
				"bak");
		std::string name = obs_data_get_string(data, "name");

		/* if no name found, use the file name as the name
		 * (this only happens when switching to the new version) */
		if (name.empty()) {
			name = strrchr(filePath, '/') + 1;
			name.resize(name.size() - 5);
		}

		obs_data_release(data);

		if (QString::fromStdString(name) != Mentor::static_name) {
			continue;
		}
		else {
			found = true;
		}

		if (!cb(name.c_str(), filePath))
			break;
	}

	os_globfree(glob);

	return found;
}

static bool SceneCollectionExists(const char *findName)
{
	bool found = false;
	auto func = [&](const char *name, const char*)
	{
		if (strcmp(name, findName) == 0) {
			found = true;
			return false;
		}

		return true;
	};

	EnumSceneCollections(func);
	return found;
}

bool check_source_files()
{
	static bool checked = false;
	if (checked)
		return true;

	QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
	bool path_ok = dir.cdUp();
	if (path_ok)
		path_ok = dir.cd(Mentor::static_name);
	if (!path_ok)
		return false;

	for (const QString& fname : QStringList({"speaker.txt", "speakers.txt", "queue1.txt", "queue2.txt"})) {
		QFile file(dir.absoluteFilePath(fname));
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			file.close();
		}
		else {
			return false;
		}
	}

	QImage img(dir.absoluteFilePath(QStringLiteral("pause.png")), "PNG");
	if (img.isNull())
		return false;

	checked = true;
	return true;
}

void OBSSimple::RefreshSceneCollections()
{
	bool sources_ok = check_source_files();

	QList<QAction*> menuActions = ui->sceneCollectionMenu->actions();
	int count = 0;

	for (int i = 0; i < menuActions.count(); i++) {
		QVariant v = menuActions[i]->property("file_name");
		if (v.typeName() != nullptr)
			delete menuActions[i];
	}

	const char *cur_name = config_get_string(App()->GlobalConfig(),
			"Basic", "SceneCollection");

	auto addCollection = [&](const char *name, const char *path)
	{
		std::string file = strrchr(path, '/') + 1;
		file.erase(file.size() - 5, 5);

		QAction *action = new QAction(QT_UTF8(name), this);
		action->setProperty("file_name", QT_UTF8(path));
		connect(action, &QAction::triggered,
				this, &OBSBasic::ChangeSceneCollection);
		action->setCheckable(true);

		action->setChecked(strcmp(name, cur_name) == 0);

		ui->sceneCollectionMenu->addAction(action);
		count++;
		return true;
	};

	bool found = EnumSceneCollections(addCollection);

	/* force saving of first scene collection on first run, otherwise
	 * no scene collections will show up */
	if (!count) {
		long prevDisableVal = disableSaving;

		disableSaving = 0;
		SaveProjectNow();
		disableSaving = prevDisableVal;

		EnumSceneCollections(addCollection);
	}

	ui->actionRemoveSceneCollection->setEnabled(count > 1);

	OBSBasic *main = reinterpret_cast<OBSBasic*>(App()->GetMainWindow());

	main->ui->actionPasteFilters->setEnabled(false);
	main->ui->actionPasteRef->setEnabled(false);
	main->ui->actionPasteDup->setEnabled(false);

	if (!found && sources_ok) {
		QDir qdir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
		bool path_ok = qdir.cdUp();
		if (path_ok)
			path_ok = qdir.cd(Mentor::static_name);
		if (path_ok) {
			QString qfilePath = qdir.absoluteFilePath(QString("%1.json").arg(Mentor::static_name));
			if (!qfilePath.isEmpty() && !qfilePath.isNull()) {
				char path[512];

				int ret = GetConfigPath(path, 512, "obs-studio/basic/scenes/");
				if (ret > 0) {
					QFileInfo finfo(qfilePath);
					QString qfilename = finfo.fileName();
					QString qpath = QT_UTF8(path);
					QFileInfo destinfo(QT_UTF8(path) + qfilename);

					string absPath = QT_TO_UTF8(finfo.absoluteFilePath());
					OBSData scenedata =
						obs_data_create_from_json_file(absPath.c_str());
					obs_data_release(scenedata);

					string origName = obs_data_get_string(scenedata, "name");
					string name = origName;
					string file;
					int inc = 1;

					while (SceneCollectionExists(name.c_str())) {
						name = origName + " (" + to_string(++inc) + ")";
					}

					obs_data_set_string(scenedata, "name", name.c_str());

					if (GetFileSafeName(name.c_str(), file)) {
						string filePath = path + file;

						if (GetClosestUnusedFileName(filePath, "json")) {
							obs_data_save_json_safe(scenedata, filePath.c_str(),
								"tmp", "bak");
							RefreshSceneCollections();
							return;
						}
						else {
							blog(LOG_WARNING, "Failed to get "
								"closest file name for %s",
								file.c_str());
						}
					}
					else {
						blog(LOG_WARNING, "Failed to create "
							"safe file name for '%s'",
							name.c_str());
					}
				}
				else {
					blog(LOG_WARNING, "Failed to get scene collection config path");
				}
			}
		}
	}

	QAction *mentorAction = Mentor::find_action(ui->sceneCollectionMenu->actions(), Mentor::static_name);
	if (mentorAction)
		mentorAction->trigger();
}
