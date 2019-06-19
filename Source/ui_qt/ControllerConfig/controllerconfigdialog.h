#pragma once

#include <QDialog>
#include <QAbstractButton>
#include <QXmlStreamReader>

#include "../input/InputBindingManager.h"
#include "InputProviderQtKey.h"

namespace Ui
{
	class ControllerConfigDialog;
}

class ControllerConfigDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ControllerConfigDialog(CInputBindingManager*, CInputProviderQtKey*, QWidget* parent = 0);
	~ControllerConfigDialog();

	static void AutoConfigureKeyboard(CInputBindingManager*);

private slots:
	void on_buttonBox_clicked(QAbstractButton* button);
	void on_tableView_doubleClicked(const QModelIndex& index);
	void on_ConfigAllButton_clicked();

	void on_comboBox_currentIndexChanged(int index);
	void on_addProfileButton_clicked();
	void on_delProfileButton_clicked();

private:
	void PrepareBindingsView();
	int OpenBindConfigDialog(int index);

	Ui::ControllerConfigDialog* ui;
	CInputBindingManager* m_inputManager = nullptr;
	CInputProviderQtKey* m_qtKeyInputProvider = nullptr;
};
