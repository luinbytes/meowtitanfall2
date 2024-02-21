
class CEngineVGui
{
public:

};

inline CEngineVGui* EngineVGUI;

class IVEngineClient013
{
private:

public:
	bool IsInGame() {
		return Memory::CallVirtual<200, bool>(this);
	}
	
	bool IsConnected() {
		return Memory::CallVirtual<201, bool>(this);
	}
	
};

inline IVEngineClient013* EngineClient;