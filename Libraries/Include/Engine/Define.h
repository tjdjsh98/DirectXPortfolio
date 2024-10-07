#pragma once

#define DECLARE_SINGLE(classname)		\
private:								\
	classname() {}						\
	classname(const classname&) {}						\
public:									\
	static classname* GetInstance()		\
	{									\
		static classname s_Instance;	\
		return &s_Instance;				\
	}									\

#define	GET_SINGLE(classname)		classname::GetInstance()

#define CHECK(p)		assert(SUCCEEDED(p))
#define GAME			GET_SINGLE(Game)
#define DC				GET_SINGLE(Game)->GetGraphics()->GetDeviceContext()
#define DEVICE			GET_SINGLE(Game)->GetGraphics()->GetDevice()
#define RESOURCE		GET_SINGLE(ResourceManager)
#define INPUT			GET_SINGLE(InputManager)
#define TIME			GET_SINGLE(TimeManager)
#define PHYSICS			GET_SINGLE(PhysicsManager)
#define SCENE			GET_SINGLE(SceneManager)
#define CUR_SCENE		SCENE->GetCurrentScene()
#define DT				TIME->GetDeltaTime()
#define INSTANCING		GET_SINGLE(InstancingManager)
#define GIZMO			GET_SINGLE(Gizmo)


#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500
