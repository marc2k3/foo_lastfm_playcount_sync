#pragma once

class ThreadedProcessCallback : public threaded_process_callback
{
public:
	ThreadedProcessCallback(metadb_handle_list_cref handles, std::string_view method);

	static void init(threaded_process_callback::ptr callback, std::string_view title = "") noexcept;

	void run(threaded_process_status& status, abort_callback& abort) noexcept final;

private:
	metadb_handle_list m_handles;
	std::string m_method;
};
