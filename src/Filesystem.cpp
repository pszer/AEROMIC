#include "Filesystem.hpp"

std::vector<std::string> GetAllFilesInDir(const std::string &dirPath) {
	std::vector<std::string> list_of_files;

	try {
		if (filesys::exists(dirPath) && filesys::is_directory(dirPath)) {
			filesys::recursive_directory_iterator iter(dirPath);

			filesys::recursive_directory_iterator end;

			while (iter != end) {
				list_of_files.push_back(iter->path().string());

				std::error_code ec;
				iter.increment(ec);
				if (ec) {
					std::cerr << "Error While Accessing : " << iter->path().string() << " :: " << ec.message() << '\n';
				}
			}
		}
	}
	catch (std::system_error & e) {
		std::cerr << "Exception : " << e.what();
	}
	return list_of_files;
}
