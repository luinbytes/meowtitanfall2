#include <string>
#include <vector>

namespace Framework {
	namespace Utils {
		inline bool opened = true;

		inline int x = 120;
		inline int y = 120;

		inline int w = 450;
		inline int h = 380;

		inline int header_height = 30;

		inline int active_tab = 0;
	}

	void Begin();

	void HandleMovement();

	int AddTabs(std::vector<std::string> tabs);
}