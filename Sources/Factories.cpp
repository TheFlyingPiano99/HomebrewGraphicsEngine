#include "Factories.h"
#include "GeometryFactory.h"
#include "MaterialFactory.h"
#include "ShaderProgramFactory.h"
#include "SceneObjectFactory.h"

void Hogra::Factories::ClearCash()
{
	GeometryFactory::GetInstance()->ForgetPointers();
	MaterialFactory::GetInstance()->ForgetPointers();
	ShaderProgramFactory::GetInstance()->ForgetPointers();
}
