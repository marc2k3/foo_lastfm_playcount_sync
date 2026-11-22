#pragma once

struct FileDialog
{
	static void open(const fb2k::fileDialogGetPath_t& path_func)
	{
		auto dialog = fb2k::fileDialog::get()->setupOpen();
		dialog->setTitle("Import from");
		dialog->setParent(fb2k::get_main_window());
		dialog->setFileTypes(types);
		dialog->runSimple(path_func);
	}

	static void save(const fb2k::fileDialogGetPath_t& path_func)
	{
		auto dialog = fb2k::fileDialog::get()->setupSave();
		dialog->setTitle("Save as");
		dialog->setDefaultExtension("json");
		dialog->setParent(fb2k::get_main_window());
		dialog->setFileTypes(types);
		dialog->runSimple(path_func);
	}

	static constexpr auto types = "JSON file|*.json|All files|*.*";
};
