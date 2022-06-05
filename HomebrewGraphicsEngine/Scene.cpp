#include "Scene.h"
#include "GlobalInclude.h"
#include "ControlActionManager.h"
#include "AssetFolderPathManager.h"
#include "TextureCube.h"
#include "GeometryFactories.h"
#include "AudioSource.h"
#include "Listener.h"

Scene* Scene::instance = nullptr;

void Scene::initCameraAndLights()
{
	camera = new Camera(contextWidth, contextHeight, glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	lights.push_back(new Light(glm::normalize(glm::vec4(-1.0f, 1.0f, -1.0f, 0.0f)), glm::vec3(0.8f, 0.8f, 0.8f)));	// Directional light
	lights.push_back(new Light(glm::vec4(10.0f, 5.0f, 20.0f, 1.0f), glm::vec3(100.0f, 100.0f, 100.0f)));
}

void Scene::initSceneObjects()
{
	Texture* cubeMap = nullptr;
	initSkyBox(&cubeMap);
	initCube(&cubeMap);
}

void Scene::initSkyBox(Texture** cubeMap)
{
	ShaderProgram* skyboxShader = new ShaderProgram(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("skybox.frag")
	);
	std::vector<std::string> images;
	images.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("right.jpg").c_str());
	images.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("left.jpg").c_str());
	images.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("top.jpg").c_str());
	images.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("bottom.jpg").c_str());
	images.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("front.jpg").c_str());
	images.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("back.jpg").c_str());
	*cubeMap = new TextureCube(images, 5);
	auto* skyBoxMaterial = new Material(skyboxShader);
	skyBoxMaterial->addTexture(*cubeMap);
	Geometry* fullscreenQuad = GeometryFactory::FullScreenQuad::getInstance();
	auto* skyBoxMesh = new Mesh(skyBoxMaterial, fullscreenQuad);
	skyBoxMesh->setDepthTest(false);
	skyBoxMesh->setStencilTest(false);
	addSceneObject(new SceneObject(skyBoxMesh));
}

void Scene::initCube(Texture** cubeMap)
{
	ShaderProgram* cubeShader = new ShaderProgram(
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.vert"),
		AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.frag")
	);
	auto* cubeMaterial = new Material(cubeShader);
	cubeMaterial->addTexture(*cubeMap);
	cubeMaterial->setReflectiveness(0.3f);
	Geometry* cubeGeometry = GeometryFactory::Cube::getInstance();
	auto* cubeMesh = new Mesh(cubeMaterial, cubeGeometry);
	auto* obj = new SceneObject(cubeMesh);
	obj->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	addSceneObject(obj);
}

void Scene::initPostProcessStages()
{
	auto* stage = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("edgeDetect.frag"),
		contextWidth, contextHeight);
	postProcessStages.push_back(stage);
}


Scene* Scene::getInstance()
{
    if (instance == nullptr) {
		throw new SceneNotInstanciatedException();
    }
    return instance;
}

void Scene::destroyInstance()
{
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}


Scene* Scene::init(int contextWidth, int contextHeight)
{
	if (instance == nullptr) {
		instance = new Scene(contextWidth, contextHeight);
		instance->initCameraAndLights();
		instance->initSceneObjects();
		instance->initPostProcessStages();
	}
	return instance;
}

void Scene::destroy()
{
	if (camera != nullptr) {
		delete camera;
		camera = nullptr;
	}

	lights.clear();

	for (auto sh : shaders) {
		sh->Delete();
		delete sh;
	}
	shaders.clear();
	for (auto& material : materials) {
		delete material;
	}
	materials.clear();

	for (auto& geometry : geometries) {
		delete geometry;
	}
	geometries.clear();

	for (auto& mesh : meshes) {
		delete mesh;
	}
	meshes.clear();

	for (auto& obj : sceneObjects) {
		delete obj;
	}
	sceneObjects.clear();

	for (auto texture : textures) {
		delete texture;
	}
	textures.clear();

	for (auto& postProcStage : postProcessStages) {
		delete postProcStage;
	}
	postProcessStages.clear();
}

//-----------------------------------------------------------------------------

void Scene::control(float dt)
{
    ControlActionManager::getInstance()->executeQueue(this, dt);

	for (auto obj : sceneObjects) {
		obj->control(dt);
	}
}

void Scene::animate(float dt)
{
	for (auto& obj : sceneObjects) {
		obj->animate(dt);
	}

	camera->update();
}

void Scene::draw()
{
	if (postProcessStages.size() > 0) {
		postProcessStages[0]->Bind();
	}
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
	glClearDepth(1);
	glDepthFunc(GL_LESS);
	glFrontFace(GL_CCW);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilMask(0x00);

	for (auto& obj : sceneObjects) {
		obj->draw(*camera, lights);
	}
	for (int i = 0; i < postProcessStages.size(); i++) {
		if (i < postProcessStages.size() - 1) {
			postProcessStages[i]->Draw(postProcessStages[i + 1]->getFBO());
		}
		else {
			postProcessStages[i]->Draw(FBO::getDefault());
		}
	}
}

void Scene::addSceneObject(SceneObject* object)
{
	if (auto objectIter = std::find(sceneObjects.begin(), sceneObjects.end(), object); objectIter != sceneObjects.end()) {		// If already contains
		return;
	}

	Mesh* mesh = object->getMesh();
	Material* material = mesh->getMaterial();
	Geometry* geometry = mesh->getGeometry();
	ShaderProgram* program = material->getShaderProgram();
	const std::vector<Texture*>& _textures = material->getTextures();

	auto meshIter = std::find(meshes.begin(), meshes.end(), mesh);
	if (meshIter == meshes.end()) {	// If does not contain
		meshes.push_back(mesh);
	}

	if (auto materialIter = std::find(materials.begin(), materials.end(), material); materialIter == materials.end()) { // If does not contain
		materials.push_back(material);
	}

	if (auto geometryIter = std::find(geometries.begin(), geometries.end(), geometry); geometryIter == geometries.end()) { // If does not contain
		geometries.push_back(geometry);
	}

	if (auto programIter = std::find(shaders.begin(), shaders.end(), program); programIter == shaders.end()) { // If does not contain
		shaders.push_back(program);
	}

	for (auto* texture : _textures) {
		auto textureIter = std::find(this->textures.begin(), this->textures.end(), texture);
		if (textureIter == this->textures.end()) { // If does not contain
			this->textures.push_back(texture);
		}
	}
	sceneObjects.push_back(object);
}

const glm::vec3& Scene::getPreferedUp() const {
	return preferedUp;
}

void Scene::togglePause()
{
	pause = !pause;
}

Camera* Scene::getCamera() {
    return camera;
}

void Scene::onContextResize(int _contextWidth, int _contextHeight)
{
	this->contextWidth = _contextWidth;
	this->contextHeight = _contextHeight;
	if (camera != nullptr) {
		camera->width = _contextWidth;
		camera->height = _contextHeight;
	}
	for (auto& stage : postProcessStages) {
		stage->resize(_contextWidth, _contextHeight);
	}
	camera->moved = true;
}

void Scene::serialize()
{
	//TODO
}
