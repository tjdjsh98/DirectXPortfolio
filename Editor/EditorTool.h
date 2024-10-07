#pragma once
#include <IExcute.h>

class EditorTool : public IExcute
{
public:
	virtual void Init() override;

	EditorTool();
	~EditorTool();

	void Awake();
	void Update();

private:
	void UpdateGUI();
private:
	shared_ptr<class CommonFileDialogApp > _fileDialog = nullptr;
	shared_ptr<class Converter> _converter = nullptr;

};

