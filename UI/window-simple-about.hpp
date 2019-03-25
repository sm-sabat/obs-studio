#pragma once

#include <memory>
#include <QDialog>

#include "ui_MentorAbout.h"

class MentorAbout : public QDialog {
	Q_OBJECT

public:
	explicit MentorAbout(QWidget *parent = 0);

private:
	std::unique_ptr<Ui::MentorAbout> ui;

private slots:
	void ShowAbout();
	void ShowAuthors();
	void ShowLicense();
};
